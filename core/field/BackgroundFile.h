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
#ifndef BACKGROUNDFILE_H
#define BACKGROUNDFILE_H

#include <QtCore>
#include <QImage>
#include "FieldPart.h"
#include "Palette.h"
#include "BackgroundTiles.h"
#include "BackgroundTextures.h"

class BackgroundFile : public FieldPart
{
public:
	explicit BackgroundFile(Field *field);
	virtual ~BackgroundFile();

	bool open() { return true; }
	bool open(const QByteArray &) { return true; }
	QByteArray save() const { return QByteArray(); }
	bool canSave() const { return false; }
	void clear() { }
	QImage openBackground();
	virtual QImage openBackground(const QHash<quint8, quint8> &paramActifs, const qint16 z[2], const bool *layers=NULL)=0;
	bool usedParams(QHash<quint8, quint8> &usedParams, bool *layerExists);
protected:
	virtual bool openTiles(const QByteArray &data)=0;
	virtual bool openTextures(const QByteArray &data, BackgroundTextures *textures) {
		Q_UNUSED(data)
		Q_UNUSED(textures)
		return false;
	}
	QImage drawBackground(const BackgroundTiles &tiles, const QList<Palette *> &palettes, const BackgroundTextures *textures) const;
	static QRgb blendColor(quint8 type, QRgb color0, QRgb color1);
	inline const BackgroundTiles &tiles() const {
		return _tiles;
	}
	inline void setTiles(const BackgroundTiles &tiles) {
		_tiles = tiles;
	}
private:
	BackgroundTiles _tiles;
};

#endif // BACKGROUNDFILE_H
