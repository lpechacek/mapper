#ifndef TP360B_DIALOG_H
#define TP360B_DIALOG_H

#include <QtBluetooth/QBluetoothSocket>
#include <QObject>
#include <QScrollArea>
#include <QString>
#include <QtGlobal>

class QTimerEvent;
class QWidget;
namespace Ui { class MainWindow; }

namespace OpenOrienteering {

class TP360BWidget : public QScrollArea
{
	Q_OBJECT

public:
	TP360BWidget(QWidget *parent = nullptr);
	~TP360BWidget();
	
private slots:
	void on_connectButton_clicked();
	void on_disconnectButton_clicked();
	void readSocket();
	void connected();
	void disconnected();
	void onSocketErrorOccurred(QBluetoothSocket::SocketError error);
	void socketStateChanged(QBluetoothSocket::SocketState state);
	
private:
	Ui::MainWindow *ui;
	QBluetoothSocket* bt_socket = {};
	int timerId = {};
	quint16 volt_readings[5] = {};
	
	void logEvent(QString str);
	void timerEvent(QTimerEvent *) override;
};

} // namespace OpenOrienteering
#endif // TP360B_DIALOG_H
