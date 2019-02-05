#ifndef OPENORIENTEERING_SYMBOL_UNDO_H
#define OPENORIENTEERING_SYMBOL_UNDO_H

#include <undo/undo.h>

namespace OpenOrienteering {

class Map;
class Symbol;

class SymbolUndoStep : public UndoStep
{
public:
    enum SymbolChange
    {
        AddSymbol      = 1,
        RemoveSymbol   = 2,
        UndefinedChange = 0
    };

	SymbolUndoStep(Map* map, SymbolChange change_type, Symbol* symbol, int pos);
	SymbolUndoStep(Map* map);

	~SymbolUndoStep() override;

	UndoStep* undo() override;
	bool isValid() const override;

protected:
	void saveImpl(QXmlStreamWriter& xml) const;
	void loadImpl(QXmlStreamReader& xml, SymbolDictionary&);

	SymbolChange change_type;
	Symbol* symbol_ptr;
	int symbol_pos;
};

}  // namespace OpenOrienteering

#endif // OPENORIENTEERING_SYMBOL_UNDO_H
