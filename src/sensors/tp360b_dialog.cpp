#include "tp360b_dialog.h"

#include <type_traits>

#include <QBluetoothAddress>
#include <QBluetoothLocalDevice>
#include <QBluetoothServiceInfo>
#include <QByteArray>
#include <QHBoxLayout>
#include <QLabel>
#include <QLatin1String>
#include <QLayout>
#include <QLayoutItem>
#include <QList>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollBar>
#include <QStringRef>
#include <QTextEdit>
#include <QUuid>
#include <QVector>
#include <QWidget>
#include <QtGlobal>

#include "../core/app_permissions.h" // FIXME - the path - remove ../
#include "tp360b_lib.h"
#include "tp360b_dialog-ui.h"

namespace {

QLatin1String const connStateNames[] {
		QLatin1String("UnconnectedState"),
		QLatin1String("ServiceLookupState"),
		QLatin1String("ConnectingState"),
		QLatin1String("ConnectedState"),
		QLatin1String("BoundState"),
		QLatin1String("ClosingState"),
		QLatin1String("ListeningState")
};


void setLayoutItemsVisible(const QLayout* layout, bool enabled)
{
	for (int i = 0; i < layout->count(); ++i)
	{
		auto* widget = layout->itemAt(i)->widget();
		if (widget)
			widget->setVisible(enabled);
	}		
}

}  // anonymous namespace


namespace OpenOrienteering {
	
TP360BWidget::TP360BWidget(QWidget *parent)
    : QScrollArea(parent)
{
	ui = new Ui::MainWindow();
	setMinimumSize(300, 150);
	ui->setupUi(this);
	
	setLayoutItemsVisible(ui->infoStripLayout, false);

	auto* local_bt_dev = new QBluetoothLocalDevice();
	if (local_bt_dev && local_bt_dev->isValid())
	{
		logEvent(QString::fromLatin1("Device %1/\"%2\" power mode %3\n")
		         .arg(local_bt_dev->address().toString(), local_bt_dev->name())
		         .arg(local_bt_dev->hostMode()));
	}
	else
	{
		logEvent(QString::fromLatin1("No valid device\n"));
	}
	auto pair_status = local_bt_dev->pairingStatus(QBluetoothAddress("00:1B:35:00:50:E6"));
	logEvent(QString::fromLatin1("Pairing status for 00:1B:35:00:50:E6: %1\n").arg(pair_status));
	for (auto const& bt_addr : local_bt_dev->connectedDevices()) {
		logEvent(QString::fromLatin1("Connected %1\n").arg(bt_addr.toString()));
	}
	connect(local_bt_dev, &QBluetoothLocalDevice::pairingDisplayConfirmation,
	        [this](const QBluetoothAddress &address, QString pin) {
		logEvent(QString::fromLatin1("pairingDisplayConfirmation: %1 %2\n").arg(address.toString(), pin));
	});
	connect(local_bt_dev, &QBluetoothLocalDevice::pairingDisplayPinCode,
	        [this](const QBluetoothAddress &address, QString pin) {
		logEvent(QString::fromLatin1("pairingDisplayPinCode: %1 %2\n").arg(address.toString(), pin));
	});
	connect(local_bt_dev, &QBluetoothLocalDevice::pairingFinished,
	        [this](const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing) {
		logEvent(QString::fromLatin1("pairingFinished: %1 %2\n").arg(address.toString()).arg(pairing));
	});
}

TP360BWidget::~TP360BWidget()
{
	delete ui;
}


void TP360BWidget::on_disconnectButton_clicked()
{
	logEvent(QString::fromLatin1("Disconnect button clicked.\n"));
	
	if (bt_socket) {
		bt_socket->disconnectFromService();
		disconnected();
		bt_socket->disconnect();
		bt_socket = nullptr;
	}
}

void TP360BWidget::on_connectButton_clicked()
{
	logEvent(QString::fromLatin1("Connect button clicked.\n"));
	
	if(AppPermissions::checkPermission(AppPermissions::Bluetooth) == AppPermissions::Denied)
	{
		logEvent(QString::fromLatin1("Missing Bluetooth permission."));
		if (AppPermissions::requestPermissionSync(AppPermissions::Bluetooth) == AppPermissions::Denied)
		{
			logEvent(QString::fromLatin1("Permisisons not granted."));
			return;
		}
	}
	
	ui->connectButton->setText(QString::fromLatin1("Connecting..."));
	
	if (bt_socket) {
		bt_socket->disconnectFromService();
		bt_socket->disconnect();
		bt_socket = nullptr;  // leave cleanup on Qt
	}
	
	// Connect to service
	bt_socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
	bt_socket->connectToService(QBluetoothAddress("00:1B:35:00:50:E6"), // FIXME: make the address configurable
	                            QUuid("{00001101-0000-1000-8000-00805f9b34fb}"));
	
	connect(bt_socket, &QBluetoothSocket::readyRead, this, &TP360BWidget::readSocket);
	connect(bt_socket, &QBluetoothSocket::connected, this, QOverload<>::of(&TP360BWidget::connected));
	connect(bt_socket, &QBluetoothSocket::disconnected, this, &TP360BWidget::disconnected);
	connect(bt_socket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error),
	        this, &TP360BWidget::onSocketErrorOccurred);
	connect(bt_socket, &QBluetoothSocket::stateChanged, this, &TP360BWidget::socketStateChanged);
	logEvent(QString::fromLatin1("Connection state: %1.\n")
	         .arg(connStateNames[bt_socket->state()]));
	
	ui->batteryCharge->reset();
}

void TP360BWidget::readSocket()
{
	if (!bt_socket)
        return;

	QString received;
	        
	while (bt_socket->canReadLine()) {
		QByteArray line = bt_socket->readLine();
		received = QString::fromLatin1(line.constData(), line.length()).trimmed();
		
		
		logEvent(QString::fromLatin1("received: \"%1\"\n").arg(received));
		
		if (received.startsWith("$BV"))
		{
			const auto voltage = received.splitRef(",")[1].toInt();
			unsigned sum = voltage;
			unsigned valid_readings = 1;
			for (int i = sizeof(volt_readings)/sizeof(volt_readings[0]) - 1; i; --i)
			{
				if (volt_readings[i])
				{
					sum += volt_readings[i];
					valid_readings++;
				}
				volt_readings[i] = volt_readings[i-1];
			}
			volt_readings[0] = voltage;
			
			ui->batteryCharge->setValue(sum/valid_readings);
		}
		else if (received.startsWith("$ID"))
		{
			const auto splits = received.splitRef(",");
			ui->fwId->setText(splits[1].toString() + QStringLiteral(", FW ")
			        + splits[2].toString());
			
			// TP360B cannot handle concurrent commands. This is somewhat primitive way
			// how to serialize $ID and $BV when the RFCOMM connection is established.
			// Given that we are issuing only one $ID, this should be fine.			
			bt_socket->write("$BV\r\n");
		}
		else if (received.size())
		{
			const auto m = LibTP360B::parseNmeaSentence(received);
			
			logEvent(QString::fromLatin1("parsed: type=%1 SD=%2 HD=%3 INC=%4 AZ=%5 accuracy=%6.\n")
			         .arg(m.type)
			         .arg(m.straightDistance)
			         .arg(m.horizontalDistance)
			         .arg(m.inclination)
			         .arg(m.azimuth)
			         .arg(m.distanceAccuracy));	
		}
	}
}

void TP360BWidget::connected()
{
	logEvent(QString::fromLatin1("Connected.\n"));
	
	bt_socket->write("$ID\r\n");
	logEvent(QString::fromLatin1("Sent ID request.\n"));
	
	timerId = startTimer(15000);
	ui->connectButton->setVisible(false);
	setLayoutItemsVisible(ui->infoStripLayout, true);
	
}

void TP360BWidget::disconnected()
{
	killTimer(timerId);
	ui->connectButton->setText(QString::fromLatin1("Connect"));
	ui->connectButton->setVisible(true);
	setLayoutItemsVisible(ui->infoStripLayout, false);
	
	logEvent(QString::fromLatin1("Disconnected.\n"));
}

void TP360BWidget::onSocketErrorOccurred(QBluetoothSocket::SocketError error)
{
	logEvent(QString::fromLatin1("Connection error: %1.\n").arg(error));
}

void TP360BWidget::socketStateChanged(QBluetoothSocket::SocketState state)
{
	logEvent(QString::fromLatin1("Connection state change: %1.\n")
	                               .arg(connStateNames[state]));
}


void TP360BWidget::timerEvent(QTimerEvent* /* unused */)
{
	bt_socket->write("$BV\r\n");
}

void TP360BWidget::logEvent(QString str)
{
	ui->logWidget->insertPlainText(str);
	ui->logWidget->verticalScrollBar()->setValue(ui->logWidget->verticalScrollBar()->maximum());
}

} // namespace OpenOrienteering
