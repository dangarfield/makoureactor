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
#include "MassExportDialog.h"

MassExportDialog::MassExportDialog(QWidget *parent) :
	QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint),
	_fieldArchive(0), _fieldID(0)
{
	setWindowTitle(tr("Exporter en masse"));

	fieldList = new QListWidget(this);
	fieldList->setUniformItemSizes(true);
	fieldList->setFixedWidth(200);
	fieldList->setSelectionMode(QAbstractItemView::MultiSelection);

	buildBgExportWidget();
	buildAkaoExportWidget();

	dirPath = new QLineEdit(this);
	changeDir = new QPushButton(tr("Choisir..."), this);

	overwriteIfExists = new QCheckBox(tr("�craser les fichiers existants"), this);
	overwriteIfExists->setChecked(Config::value("overwriteOnExport", true).toBool());

	QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
	buttonBox->addButton(tr("Exporter"), QDialogButtonBox::AcceptRole);
	buttonBox->addButton(QDialogButtonBox::Cancel);

	QGridLayout *layout = new QGridLayout(this);
	layout->addWidget(fieldList, 0, 0, 6, 1);
	layout->addWidget(bgExport, 0, 1, 1, 2);
	layout->addWidget(akaoExport, 1, 1, 1, 2);
	layout->addWidget(new QLabel(tr("Emplacement de l'export :")), 2, 1, 1, 2);
	layout->addWidget(dirPath, 3, 1);
	layout->addWidget(changeDir, 3, 2);
	layout->addWidget(overwriteIfExists, 4, 1, 1, 2);
	layout->addWidget(buttonBox, 6, 0, 1, 3, Qt::AlignRight);
	layout->setRowStretch(5, 1);
	layout->setColumnStretch(1, 1);

	connect(changeDir, SIGNAL(clicked()),  SLOT(chooseExportDirectory()));
	connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));
}

void MassExportDialog::buildBgExportWidget()
{
	bgExport = new QGroupBox(tr("Exporter les d�cors"), this);
	bgExport->setCheckable(true);
	bgExport->setChecked(true);

	bgFormat = new QComboBox(bgExport);
	bgFormat->addItem(tr("Image PNG"));
	bgFormat->addItem(tr("Image JPG"));
	bgFormat->addItem(tr("Image BMP"));
	bgFormat->setCurrentIndex(Config::value("exportBackgroundFormat").toInt());

	QVBoxLayout *layout = new QVBoxLayout(bgExport);
	layout->addWidget(bgFormat);
}

void MassExportDialog::buildAkaoExportWidget()
{
	akaoExport = new QGroupBox(tr("Exporter les sons"), this);
	akaoExport->setCheckable(true);
	akaoExport->setChecked(false);

	akaoFormat = new QComboBox(akaoExport);
	akaoFormat->addItem(tr("Son AKAO"));
	akaoFormat->setEnabled(false);

	QVBoxLayout *layout = new QVBoxLayout(akaoExport);
	layout->addWidget(akaoFormat);
}

void MassExportDialog::fill(FieldArchive *fieldArchive, int fieldID)
{
	_fieldArchive = fieldArchive;
	_fieldID = fieldID;

	fieldList->clear();
	for(int i=0 ; i<_fieldArchive->size() ; ++i) {
		Field *field = _fieldArchive->field(i, false);
		if(field) {
			QListWidgetItem *item = new QListWidgetItem(field->getName());
			item->setData(Qt::UserRole, i);
			fieldList->addItem(item);
		}
	}
	fieldList->selectAll();
}

void MassExportDialog::chooseExportDirectory()
{
	QString dir = Config::value("exportDirectory").toString();
	dir = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier"), dir);
	if(dir.isNull())	return;

	Config::setValue("exportDirectory", dir);
	dirPath->setText(dir);
}

QList<int> MassExportDialog::selectedFields() const
{
	QList<int> ids;

	QList<QListWidgetItem *> items = fieldList->selectedItems();

	foreach(QListWidgetItem *item, items) {
		ids.append(item->data(Qt::UserRole).toInt());
	}

	return ids;
}

bool MassExportDialog::exportBackground() const
{
	return bgExport->isChecked();
}

int MassExportDialog::exportBackgroundFormat() const
{
	return bgFormat->currentIndex();
}

bool MassExportDialog::exportAkao() const
{
	return akaoExport->isChecked();
}

int MassExportDialog::exportAkaoFormat() const
{
	return akaoFormat->currentIndex();
}

QString MassExportDialog::directory() const
{
	return dirPath->text();
}

bool MassExportDialog::overwrite() const
{
	return overwriteIfExists->isChecked();
}

void MassExportDialog::accept()
{
	Config::setValue("overwriteOnExport", overwrite());
	Config::setValue("exportBackgroundFormat", exportBackgroundFormat());

	QDialog::accept();
}