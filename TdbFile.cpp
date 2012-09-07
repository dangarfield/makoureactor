#include "TdbFile.h"
#include <QImage>

TdbFile::TdbFile()
{
}

bool TdbFile::open(const QByteArray &data)
{
	const char *constData = data.constData();

	memcpy(&header, constData, sizeof(TdbHeader));

	if(header.size != (quint32)data.size()) {
		qWarning() << "invalid tdb size" << header.size << data.size();
		return false;
	}

	this->data = data;

//	qDebug() << "TdbFile" << header.imageOffset << header.imageCount << header.paletteOffset << header.paletteCount << sizeof(TdbHeader) << header.size;

//	quint8 faceID = 0;

//	for(int pal=0 ; pal<header.paletteCount ; ++pal) {
//		quint32 offsetPalette = header.paletteOffset + pal * 32;
//		QImage palette(64, 4, QImage::Format_RGB32);

//		for(int i=0 ; i<16 ; ++i) {
//			quint16 color;
//			memcpy(&color, &constData[offsetPalette + i*2], 2);
//			palette.setPixel(i*4+0, 0, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+1, 0, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+2, 0, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+3, 0, PsColor::fromPsColor(color));

//			palette.setPixel(i*4+0, 1, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+1, 1, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+2, 1, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+3, 1, PsColor::fromPsColor(color));

//			palette.setPixel(i*4+0, 2, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+1, 2, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+2, 2, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+3, 2, PsColor::fromPsColor(color));

//			palette.setPixel(i*4+0, 3, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+1, 3, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+2, 3, PsColor::fromPsColor(color));
//			palette.setPixel(i*4+3, 3, PsColor::fromPsColor(color));
//		}

////		QFile::remove(QString("TdbPal%1.png").arg(pal));
//		palette.save(QString("TdbPal%1.png").arg(pal));
//	}

//	for(quint8 faceID=0 ; faceID < 0x21 ; ++faceID) {
//		texture(faceID, Eye).save(QString("TdbTexEye%1.png").arg(faceID));
////		texture(faceID, MouthClosed).save(QString("TdbTexMouthClosed%1.png").arg(faceID));
//	}

	return true;
}

QPixmap TdbFile::texture(quint8 faceID, TextureType type)
{
	if(data.isEmpty())	return QPixmap();

	const char *constData = data.constData();
	int imgID = faceIdToImageId(faceID, type);

//	qDebug() << "faceID" << faceID << "imgID" << imgID;

	quint32 offsetImage = header.imageOffset + imgID * 512;

	int pal = qMin((quint16)faceID, quint16(header.paletteCount-1));

	quint32 offsetPalette = header.paletteOffset + pal * 32;

	QImage img(32, 32, QImage::Format_ARGB32);
	QRgb *px = (QRgb *)img.bits();

	for(int i=0 ; i<512 ; ++i) {
		quint8 index = constData[offsetImage + i];
		quint16 color;
		memcpy(&color, &constData[offsetPalette + (index & 0x0F)*2], 2);
		px[i*2] = PsColor::fromPsColor(color, true);

		memcpy(&color, &constData[offsetPalette + (index >> 4)*2], 2);
		px[i*2+1] = PsColor::fromPsColor(color, true);
	}

	return QPixmap::fromImage(img);
}

int TdbFile::faceIdToImageId(quint8 faceID, TextureType type)
{
	if(faceID <= 9) {
		if(type == Empty)	return 126;
		return faceID * 8 + (quint8)type;
	} else {
		switch(type) {
		case Eye:
		case EyeOpened1:case EyeOpened2:
			return 10 * 8 + (faceID - 10) * 2;
		case EyeClosed1:case EyeClosed2:
			return 126;
		case MouthClosed:
			return 126;
		case MouthOpened:
			return 10 * 8 + (faceID - 10) * 2 + 1;
		default:
			return 126;
		}
	}
}
