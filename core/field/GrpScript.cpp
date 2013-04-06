/****************************************************************************
 ** Makou Reactor Final Fantasy VII Field Script Editor
 ** Copyright (C) 2009-2012 Arzel J�r�me <myst6re@gmail.com>
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "GrpScript.h"
#include "../FF7Text.h"

GrpScript::GrpScript() :
	character(-1), animation(false), location(false), director(false)
{
	addScript(QByteArray(2, '\x0'));
	for(int i=1 ; i<32 ; i++)	addScript();
}

GrpScript::GrpScript(const QString &name) :
	_name(name), character(-1), animation(false), location(false), director(false)
{
}

GrpScript::GrpScript(const GrpScript &other) :
	_name(other.realName()), character(-1), animation(false), location(false), director(false)
{
	foreach(Script *script, other.scripts()) {
		_scripts.append(new Script(*script));
	}
}

GrpScript::~GrpScript()
{
	foreach(Script *script, _scripts)	delete script;
}

void GrpScript::addScript()
{
	if(_scripts.isEmpty())	_scripts.append(new Script());
	_scripts.append(new Script());
}

bool GrpScript::addScript(const QByteArray &script, bool explodeInit)
{
	Script *s;

	if(explodeInit && _scripts.isEmpty())
	{
		s = new Script(script);
		if(!s->isValid()) {
			delete s;
			return false;
		}
		_scripts.append(s); // S0 - Init
		_scripts.append(s->splitScriptAtReturn()); // S0 - Main
	}
	else {
		s = new Script(script);
		if(!s->isValid()) {
			delete s;
			return false;
		}
		_scripts.append(s);
	}

	return true;
}

/* void GrpScript::replaceScript(int row, QByteArray script)
{
	_scripts.replace(row, new Script(script));
} */

void GrpScript::setType()
{
	if(_scripts.isEmpty())	return;
	character = -1;
	director = animation = location = false;
	
	Script *firstScript = _scripts.first();
	
	foreach(Opcode *opcode, firstScript->getOpcodes())
	{
		switch((Opcode::Keys)opcode->id())
		{
		case Opcode::PC://Definition du personnage
			character = ((OpcodePC *)opcode)->charID;
			return;
		case Opcode::CHAR://Definition du mod�le 3D
			character = 0xFF;
			break;
		case Opcode::LINE://definition d'une zone
			if(character==-1)	location = true;
			return;
		case Opcode::BGPDH:case Opcode::BGSCR:case Opcode::BGON:
		case Opcode::BGOFF:case Opcode::BGROL:case Opcode::BGROL2:
		case Opcode::BGCLR://bg param�tres
			if(character==-1)	animation = true;
			return;
		case Opcode::MPNAM://mapname
			if(character==-1)	director = true;
			return;
		default:
			break;
		}
	}
}

void GrpScript::backgroundParams(QHash<quint8, quint8> &paramActifs) const
{
	if(_scripts.isEmpty())	return;

	_scripts.first()->backgroundParams(paramActifs);
}

void GrpScript::backgroundMove(qint16 z[2], qint16 *x, qint16 *y) const
{
	if(_scripts.size()<2)	return;

	foreach(Script *script, _scripts) {
		script->backgroundMove(z, x, y);
	}
}

QString GrpScript::name() const
{
	return _name.isEmpty() ? QObject::tr("Sans nom") : _name;
}

void GrpScript::setName(const QString &name)
{
	_name = name;
}

const QString &GrpScript::realName() const
{
	return _name;
}

int GrpScript::size() const
{
	return _scripts.size();
}

Script *GrpScript::script(quint8 scriptID) const
{
	return _scripts.value(scriptID);
}

const QList<Script *> &GrpScript::scripts() const
{
	return _scripts;
}

QByteArray GrpScript::toByteArray(quint8 scriptID) const
{
	if(scriptID == 0)
	{
		if(!_scripts.isEmpty() && !_scripts.first()->isEmpty())
			return _scripts.first()->toByteArray()+_scripts.at(1)->toByteArray();
		return QByteArray();
	}
	if(scriptID+1 < _scripts.size())
		return _scripts.at(scriptID+1)->toByteArray();
	return QByteArray();
}

GrpScript::Type GrpScript::typeID()
{
	setType();
	if(character != -1)		return Model;
	if(location)			return Location;
	if(animation)			return Animation;
	if(director)			return Director;
	return NoType;
}

QString GrpScript::type()
{
	switch(typeID())
	{
	case Model:
		if(character == 0xFF)	return QObject::tr("Objet 3D");
		return QString("%1").arg(Opcode::_personnage(character));
	case Location:	return QObject::tr("Zone");
	case Animation:	return QObject::tr("Animation");
	case Director:	return QObject::tr("Main");
	default:		return QString();
	}
}

QColor GrpScript::typeColor()
{
	switch(typeID())
	{
	case Model:		return QColor(0x00,0x66,0xcc);
	case Location:	return QColor(0x00,0xcc,0x66);
	case Animation:	return QColor(0xcc,0x66,0x00);
	case Director:	return QColor(0x66,0x00,0xcc);
	default:		return QColor();
	}
}

QString GrpScript::scriptName(quint8 scriptID)
{
	Type type = typeID();
	
	switch(scriptID)
	{
	case 0:	return QObject::tr("S0 - Init");
	case 1:	return QObject::tr("S0 - Main");
	case 2:
		if(type == Model)		return QObject::tr("S1 - Parler");
		if(type == Location)	return QObject::tr("S1 - [OK]");
		break;
	case 3:
		if(type == Model)		return QObject::tr("S2 - Toucher");
		if(type == Location)	return QObject::tr("S2 - Bouger");
		break;
	case 4:
		if(type == Location)	return QObject::tr("S3 - Bouger");
		break;
	case 5:
		if(type == Location)	return QObject::tr("S4 - Aller");
		break;
	case 6:
		if(type == Location)	return QObject::tr("S5 - Aller 1x");
		break;
	case 7:
		if(type == Location)	return QObject::tr("S6 - Partir");
		break;
	}
	
	return QObject::tr("Script %1").arg(scriptID-1);
}

bool GrpScript::search(int &scriptID, int &opcodeID) const
{
	if(scriptID < 0)
		opcodeID = scriptID = 0;
	if(scriptID >= _scripts.size())
		return false;
	return true;
}

bool GrpScript::searchOpcode(int opcode, int &scriptID, int &opcodeID) const
{
	if(!search(scriptID, opcodeID))
		return false;
	if(_scripts.at(scriptID)->searchOpcode(opcode, opcodeID))
		return true;

	return searchOpcode(opcode, ++scriptID, opcodeID = 0);
}

bool GrpScript::searchVar(quint8 bank, quint8 adress, int value, int &scriptID, int &opcodeID) const
{
	if(!search(scriptID, opcodeID))
		return false;
	if(_scripts.at(scriptID)->searchVar(bank, adress, value, opcodeID))
		return true;

	return searchVar(bank, adress, value, ++scriptID, opcodeID = 0);
}

void GrpScript::searchAllVars(QList<FF7Var> &vars) const
{
	foreach(Script *script, _scripts) {
		script->searchAllVars(vars);
	}
}

bool GrpScript::searchExec(quint8 group, quint8 script, int &scriptID, int &opcodeID) const
{
	if(!search(scriptID, opcodeID))
		return false;
	if(_scripts.at(scriptID)->searchExec(group, script, opcodeID))
		return true;

	return searchExec(group, script, ++scriptID, opcodeID = 0);
}

bool GrpScript::searchMapJump(quint16 field, int &scriptID, int &opcodeID) const
{
	if(!search(scriptID, opcodeID))
		return false;
	if(_scripts.at(scriptID)->searchMapJump(field, opcodeID))
		return true;

	return searchMapJump(field, ++scriptID, opcodeID = 0);
}

bool GrpScript::searchTextInScripts(const QRegExp &text, int &scriptID, int &opcodeID) const
{
	if(scriptID < 0)
		opcodeID = scriptID = 0;
	if(scriptID >= _scripts.size())
		return false;
	if(_scripts.at(scriptID)->searchTextInScripts(text, opcodeID))
		return true;

	return searchTextInScripts(text, ++scriptID, opcodeID = 0);
}

bool GrpScript::searchP(int &scriptID, int &opcodeID) const
{
	if(scriptID >= _scripts.size()) {
		scriptID = _scripts.size()-1;
		opcodeID = 2147483647;
	}
	if(scriptID < 0)
		return false;
	return true;
}

bool GrpScript::searchOpcodeP(int opcode, int &scriptID, int &opcodeID) const
{
	if(!searchP(scriptID, opcodeID))
		return false;
	if(_scripts.at(scriptID)->searchOpcodeP(opcode, opcodeID))
		return true;

	return searchOpcodeP(opcode, --scriptID, opcodeID = 2147483647);
}

bool GrpScript::searchVarP(quint8 bank, quint8 adress, int value, int &scriptID, int &opcodeID) const
{
	if(!searchP(scriptID, opcodeID))
		return false;
	if(_scripts.at(scriptID)->searchVarP(bank, adress, value, opcodeID))
		return true;

	return searchVarP(bank, adress, value, --scriptID, opcodeID = 2147483647);
}

bool GrpScript::searchExecP(quint8 group, quint8 script, int &scriptID, int &opcodeID) const
{
	if(!searchP(scriptID, opcodeID))
		return false;
	if(_scripts.at(scriptID)->searchExecP(group, script, opcodeID))
		return true;

	return searchExecP(group, script, --scriptID, opcodeID = 2147483647);
}

bool GrpScript::searchMapJumpP(quint16 field, int &scriptID, int &opcodeID) const
{
	if(!searchP(scriptID, opcodeID))
		return false;
	if(_scripts.at(scriptID)->searchMapJumpP(field, opcodeID))
		return true;

	return searchMapJumpP(field, --scriptID, opcodeID = 2147483647);
}

bool GrpScript::searchTextInScriptsP(const QRegExp &text, int &scriptID, int &opcodeID) const
{
	if(!searchP(scriptID, opcodeID))
		return false;
	if(_scripts.at(scriptID)->searchTextInScriptsP(text, opcodeID))
		return true;

	return searchTextInScriptsP(text, --scriptID, opcodeID = 2147483647);
}

void GrpScript::listUsedTexts(QSet<quint8> &usedTexts) const
{
	foreach(Script *script, _scripts)
		script->listUsedTexts(usedTexts);
}

void GrpScript::listUsedTuts(QSet<quint8> &usedTuts) const
{
	foreach(Script *script, _scripts)
		script->listUsedTuts(usedTuts);
}

void GrpScript::shiftTextIds(int textId, int steps)
{
	foreach(Script *script, _scripts)
		script->shiftTextIds(textId, steps);
}

void GrpScript::shiftTutIds(int tutId, int steps)
{
	foreach(Script *script, _scripts)
		script->shiftTutIds(tutId, steps);
}

void GrpScript::setWindow(const FF7Window &win)
{
	if(win.scriptID < _scripts.size()) {
		_scripts.at(win.scriptID)->setWindow(win);
	}
}

void GrpScript::listWindows(int groupID, QMultiMap<quint64, FF7Window> &windows, QMultiMap<quint8, quint64> &text2win) const
{
	int scriptID=0;
	foreach(Script *script, _scripts)
		script->listWindows(groupID, scriptID++, windows, text2win);
}