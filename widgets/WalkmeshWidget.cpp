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
#include "WalkmeshWidget.h"

WalkmeshWidget::WalkmeshWidget(QWidget *parent, const QGLWidget *shareWidget) :
	QGLViewer(/*QGLFormat(QGL::SampleBuffers),*/ parent, shareWidget),
	distance(0.0f), xRot(0.0f), yRot(0.0f), zRot(0.0f),
	xTrans(0.0f), yTrans(0.0f), transStep(360.0f), lastKeyPressed(-1),
	camID(0), _selectedTriangle(-1), _selectedDoor(-1), _selectedGate(-1),
	_selectedArrow(-1), fovy(70.0), walkmesh(0), _camera(0), infFile(0),
	bgFile(0), scripts(0), field(0)
{
	setMinimumSize(320, 240);
//	setAutoFillBackground(false);
//	arrow = QPixmap(":/images/field-arrow-red.png");
}

void WalkmeshWidget::clear()
{
	walkmesh = 0;
	_camera = 0;
	infFile = 0;
	bgFile = 0;
	scripts = 0;
	field = 0;
	updateGL();
}

void WalkmeshWidget::fill(Field *field)
{
	this->walkmesh = field->walkmesh();
	this->_camera = field->camera();
	this->infFile = field->inf();
	this->bgFile = field->background();
	this->scripts = field->scriptsAndTexts();
	this->field = field;
	updatePerspective();
	resetCamera();

	if(_camera->isOpen() && _camera->hasCamera() && camID < _camera->cameraCount()) {
		const Camera &cam = _camera->camera(camID);

		double camAxisXx = cam.camera_axis[0].x/4096.0;
		double camAxisXy = cam.camera_axis[0].y/4096.0;
		double camAxisXz = cam.camera_axis[0].z/4096.0;

		double camAxisYx = -cam.camera_axis[1].x/4096.0;
		double camAxisYy = -cam.camera_axis[1].y/4096.0;
		double camAxisYz = -cam.camera_axis[1].z/4096.0;

		double camAxisZx = cam.camera_axis[2].x/4096.0;
		double camAxisZy = cam.camera_axis[2].y/4096.0;
		double camAxisZz = cam.camera_axis[2].z/4096.0;

		double camPosX = cam.camera_position[0]/4096.0;
		double camPosY = -cam.camera_position[1]/4096.0;
		double camPosZ = cam.camera_position[2]/4096.0;

		double tx = -(camPosX*camAxisXx + camPosY*camAxisYx + camPosZ*camAxisZx);
		double ty = -(camPosX*camAxisXy + camPosY*camAxisYy + camPosZ*camAxisZy);
		double tz = -(camPosX*camAxisXz + camPosY*camAxisYz + camPosZ*camAxisZz);


		double m[3][3];
		float downScaler = 4096.0/*scripts->isOpen() ? scripts->scale() / 4.0 : 128.0*/;
		qglviewer::Quaternion orientation;
		qglviewer::Vec position(cam.camera_position[0]/downScaler, -cam.camera_position[1]/downScaler, cam.camera_position[2]/downScaler);

		m[0][0] = camAxisXx;
		m[1][0] = camAxisXy;
		m[2][0] = camAxisXz;
		m[0][1] = camAxisYx;
		m[1][1] = camAxisYy;
		m[2][1] = camAxisYz;
		m[0][2] = camAxisZx;
		m[1][2] = camAxisZy;
		m[2][2] = camAxisZz;

		orientation.setFromRotationMatrix(m);
		position = orientation * position;

		m[0][0] = m[0][0];
		m[1][0] = m[1][0];
		m[2][0] = m[2][0];
		m[0][1] = m[0][1];
		m[1][1] = m[1][1];
		m[2][1] = m[2][1];
		m[0][2] = -m[0][2];
		m[1][2] = -m[1][2];
		m[2][2] = -m[2][2];
		orientation.setFromRotationMatrix(m);

		qDebug() << "setCam";
		camera()->setScreenWidthAndHeight(320, 240);
//		camera()->setFieldOfView(2 * atanf(240.0f / (2.0f * cam.camera_zoom))/* * 57.29577951*/);
		camera()->setPosition(position);
		camera()->setOrientation(orientation);

//		camera()->setViewDirection(qglviewer::Vec(tx + camAxisZx, ty + camAxisZy, tz + camAxisZz));
//		camera()->setUpVector(qglviewer::Vec(camAxisYx, camAxisYy, camAxisYz));
//		gluLookAt(tx, ty, tz, tx + camAxisZx, ty + camAxisZy, tz + camAxisZz, camAxisYx, camAxisYy, camAxisYz);
	}
}

void WalkmeshWidget::computeFov()
{
	if(_camera && _camera->isOpen()
			&& _camera->hasCamera()
			&& camID < _camera->cameraCount()) {
		const Camera &cam = _camera->camera(camID);
		fovy = (2 * atan(240.0/(2.0 * cam.camera_zoom))) * 57.29577951;
	} else {
		fovy = 70.0;
	}
}

void WalkmeshWidget::updatePerspective()
{
	computeFov();
	resizeGL(width(), height());
	updateGL();
}

void WalkmeshWidget::init()
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_COLOR_MATERIAL);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/*void WalkmeshWidget::resizeGL(int width, int height)
{
//	glViewport(0, 0, width, height);

//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();

//	gluPerspective(fovy, (double)width/(double)height, 0.001, 1000.0);

//	glMatrixMode(GL_MODELVIEW);
}*/

void WalkmeshWidget::drawWithNames()
{
	qDebug() << "drawWithNames()";
	int i=0;

	glBegin(GL_LINES);

	foreach(const Triangle &triangle, walkmesh->getTriangles()) {
		const Access &access = walkmesh->access(i);
		glPushName(i);
		drawId(i, triangle, access);
		glPopName();
		++i;
	}

	glEnd();
}


void WalkmeshWidget::draw()
{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();

	if(!walkmesh)	return;

//	glTranslatef(xTrans, yTrans, distance);

//	QPainter painter;
//	painter.begin(this);
//	painter.setRenderHint(QPainter::Antialiasing);
//	int arrowW, arrowH;
//	if(width() < height()) {
//		arrowW = width() * 9 / 320;
//		arrowH = width() * 6 / 320;
//	} else {
//		arrowW = height() * 9 / 240;
//		arrowH = height() * 6 / 240;
//	}
//	painter.drawPixmap(0, 0, arrowW, arrowH, arrow);
//	painter.end();


//	glRotatef(xRot, 1.0, 0.0, 0.0);
//	glRotatef(yRot, 0.0, 1.0, 0.0);
//	glRotatef(zRot, 0.0, 0.0, 1.0);

	/*if(bgFile) {
		glColor3ub(255, 255, 255);
		QPixmap pix = bgFile->openBackground();
		glEnable(GL_TEXTURE_2D);
		qDebug() << pix.width() << pix.height() << ((-pix.width() / 2) / 4096.0f) << ((-pix.height() / 2) / 4096.0f);
		GLuint texId = bindTexture(pix, GL_TEXTURE_2D, GL_RGB, QGLContext::MipmapBindOption);

		glBegin(GL_QUADS);
//		glColor3ub(255, 255, 255);
		glTexCoord2d(0.0f, 0.0f);
		glVertex3f((-pix.width() / 1024.0f), (pix.height() / 1024.0f), -4095.0f / 4096.0f);

//		glColor3ub(255, 255, 0);
		glTexCoord2d(1.0f, 0.0f);
		glVertex3f((pix.width() / 1024.0f), (pix.height() / 1024.0f), -4095.0f / 4096.0f);

//		glColor3ub(0, 0, 255);
		glTexCoord2d(1.0f, 1.0f);
		glVertex3f((pix.width() / 1024.0f), (-pix.height() / 1024.0f), -4095.0f / 4096.0f);

//		glColor3ub(0, 255, 0);
		glTexCoord2d(0.0f, 1.0f);
		glVertex3f((-pix.width() / 1024.0f), (-pix.height() / 1024.0f), -4095.0f / 4096.0f);

		glEnd();

		deleteTexture(texId);
		glDisable(GL_TEXTURE_2D);
	}*/

	if(walkmesh->isOpen()) {

//		if(infFile && infFile->isOpen()) {
//			glBegin(GL_QUADS);
//			glColor3ub(255, 0, 0);

//			float arrowW, arrowH;
//			if(width() < height()) {
//				arrowW = width() * 9.0f / 320.0f;
//				arrowH = width() * 6.0f / 320.0f;
//			} else {
//				arrowW = height() * 9.0f / 240.0f;
//				arrowH = height() * 6.0f / 240.0f;
//			}

//			int gateID=0;
//			foreach(const Exit &gate, infFile->exitLines()) {
//				if(gate.fieldID != 0x7FFF && infFile->arrowIsDisplayed(gateID)) {
//					Vertex_s vertex1, vertex2, vertex3;
//					vertex1 = gate.exit_line[0];
//					vertex2 = gate.exit_line[1];
//					vertex3.x = ((vertex1.x - vertex2.x) / 2.0f) / 4096.0f;
//					vertex3.y = ((vertex1.y - vertex2.y) / 2.0f) / 4096.0f;
//					vertex3.z = ((vertex1.z - vertex2.z) / 2.0f) / 4096.0f;

//					glVertex3f(vertex1.x, vertex1.y, vertex3.z);
//					glVertex3f(vertex1.x, vertex2.y, vertex3.z);
//					glVertex3f(vertex2.x, vertex1.y, vertex3.z);
//					glVertex3f(vertex2.x, vertex2.y, vertex3.z);
//				}
//				++gateID;
//			}

//			glEnd();
//		}


		/*glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		glColor3f(0.0, 0.0, 0.5);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(-1.0, 0.0, 0.0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);
		glColor3f(0.0, 0.5, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, -1.0, 0.0);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);
		glColor3f(0.5, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, -1.0);
		glEnd();*/

		glColor3ub(0xFF, 0xFF, 0xFF);

		glBegin(GL_LINES);

		int i=0;

		foreach(const Triangle &triangle, walkmesh->getTriangles()) {
			const Access &access = walkmesh->access(i);
			drawId(i, triangle, access);

			++i;
		}

		if(infFile && infFile->isOpen()) {
			glColor3ub(0xFF, 0x00, 0x00);
//			glEnable(GL_TEXTURE_2D);
//			GLuint texID = bindTexture(arrow);

			int gateID=0;
			foreach(const Exit &gate, infFile->exitLines()) {
				if(gate.fieldID != 0x7FFF) {
					Vertex_s vertex;
					vertex = gate.exit_line[0];
					glVertex3f(vertex.x/4096.0f, vertex.y/4096.0f, vertex.z/4096.0f);
					vertex = gate.exit_line[1];
					glVertex3f(vertex.x/4096.0f, vertex.y/4096.0f, vertex.z/4096.0f);
					if(infFile->arrowIsDisplayed(gateID)) {
//						glTexCoord2d(0.0, 1.0);
//						glTexCoord2d(1.0, 0.0);
					}
				}
				++gateID;
			}

//			deleteTexture(texID);
//			glDisable(GL_TEXTURE_2D);

			glColor3ub(0x00, 0xFF, 0x00);

			foreach(const Trigger &trigger, infFile->triggers()) {
				if(trigger.background_parameter != 0xFF) {
					Vertex_s vertex;
					vertex = trigger.trigger_line[0];
					glVertex3f(vertex.x/4096.0f, vertex.y/4096.0f, vertex.z/4096.0f);
					vertex = trigger.trigger_line[1];
					glVertex3f(vertex.x/4096.0f, vertex.y/4096.0f, vertex.z/4096.0f);
				}
			}
		}

		glEnd();

		glPointSize(7.0);

		glBegin(GL_POINTS);

		glColor3ub(0xFF, 0x90, 0x00);

		if(_selectedTriangle >= 0 && _selectedTriangle < walkmesh->triangleCount()) {
			const Triangle &triangle = walkmesh->triangle(_selectedTriangle);
			const Vertex_sr &vertex1 = triangle.vertices[0];
			glVertex3f(vertex1.x/4096.0f, vertex1.y/4096.0f, vertex1.z/4096.0f);
			const Vertex_sr &vertex2 = triangle.vertices[1];
			glVertex3f(vertex2.x/4096.0f, vertex2.y/4096.0f, vertex2.z/4096.0f);
			const Vertex_sr &vertex3 = triangle.vertices[2];
			glVertex3f(vertex3.x/4096.0f, vertex3.y/4096.0f, vertex3.z/4096.0f);
		}

		if(infFile && infFile->isOpen()) {
			glColor3ub(0xFF, 0x00, 0x00);

			if(_selectedGate >= 0 && _selectedGate < 12) {
				const Exit &gate = infFile->exitLine(_selectedGate);
				if(gate.fieldID != 0x7FFF) {
					const Vertex_s &vertex1 = gate.exit_line[0];
					glVertex3f(vertex1.x/4096.0f, vertex1.y/4096.0f, vertex1.z/4096.0f);
					const Vertex_s &vertex2 = gate.exit_line[1];
					glVertex3f(vertex2.x/4096.0f, vertex2.y/4096.0f, vertex2.z/4096.0f);
				}
			}

			glColor3ub(0x00, 0xFF, 0x00);

			if(_selectedDoor >= 0 && _selectedDoor < 12) {
				const Trigger &trigger = infFile->trigger(_selectedDoor);
				if(trigger.background_parameter != 0xFF) {
					const Vertex_s &vertex1 = trigger.trigger_line[0];
					glVertex3f(vertex1.x/4096.0f, vertex1.y/4096.0f, vertex1.z/4096.0f);
					const Vertex_s &vertex2 = trigger.trigger_line[1];
					glVertex3f(vertex2.x/4096.0f, vertex2.y/4096.0f, vertex2.z/4096.0f);
				}
			}
		}

		glEnd();
	}

	/*if(scripts && scripts->isOpen()) {
		QMultiMap<int, FF7Position> positions;
		scripts->listModelPositions(positions);
		if(!positions.isEmpty()) {
			QMapIterator<int, FF7Position> i(positions);
			while(i.hasNext()) {
				i.next();
				const int modelId = i.key();
				const FF7Position &position = i.value();

				FieldModelFile *fieldModel = field->fieldModel(modelId);

			}
		}
	}*/
}

void WalkmeshWidget::drawId(int triangleID, const Triangle &triangle, const Access &access)
{
//	glColor3ub(0xFF, 0xFF, 0xFF);
//	glVertex3f(triangle.vertices[0].x/4096.0f, triangle.vertices[0].y/4096.0f, triangle.vertices[0].z/4096.0f);
//	glVertex3f(triangle.vertices[1].x/4096.0f, triangle.vertices[1].y/4096.0f, triangle.vertices[1].z/4096.0f);
//	glVertex3f(triangle.vertices[2].x/4096.0f, triangle.vertices[2].y/4096.0f, triangle.vertices[2].z/4096.0f);
	drawIdLine(triangleID, triangle.vertices[0], triangle.vertices[1], access.a[0]);
	drawIdLine(triangleID, triangle.vertices[1], triangle.vertices[2], access.a[1]);
	drawIdLine(triangleID, triangle.vertices[2], triangle.vertices[0], access.a[2]);
}

void WalkmeshWidget::drawIdLine(int triangleID, const Vertex_sr &vertex1, const Vertex_sr &vertex2, qint16 access)
{
	if(triangleID == _selectedTriangle) {
		glColor3ub(0xFF, 0x90, 0x00);
	} else if(access == -1) {
		glColor3ub(0x66, 0x99, 0xCC);
	} else {
		glColor3ub(0xFF, 0xFF, 0xFF);
	}

	glVertex3f(vertex1.x/4096.0f, vertex1.y/4096.0f, vertex1.z/4096.0f);
	glVertex3f(vertex2.x/4096.0f, vertex2.y/4096.0f, vertex2.z/4096.0f);
}

void WalkmeshWidget::wheelEvent(QWheelEvent *event)
{
	/*setFocus();
	distance += event->delta() / 4096.0;
	updateGL();*/
	QGLViewer::wheelEvent(event);
}

void WalkmeshWidget::mousePressEvent(QMouseEvent *event)
{
	/*setFocus();
	if(event->button() == Qt::MidButton)
	{
		resetCamera();
		updateGL();
	}
	else if(event->button() == Qt::LeftButton)
	{
		moveStart = event->pos();
	}*/
	QGLViewer::mousePressEvent(event);
}

void WalkmeshWidget::mouseMoveEvent(QMouseEvent *event)
{
	/*xTrans += (event->pos().x() - moveStart.x()) / 4096.0;
	yTrans -= (event->pos().y() - moveStart.y()) / 4096.0;
	moveStart = event->pos();
	updateGL();*/
	QGLViewer::mouseMoveEvent(event);
}

void WalkmeshWidget::postSelection(const QPoint &point)
{
	qglviewer::Vec orig, dir, selectedPoint;
	// Compute orig and dir, used to draw a representation of the intersecting line
	camera()->convertClickToLine(point, orig, dir);

	// Find the selectedPoint coordinates, using camera()->pointUnderPixel().
	bool found;
	selectedPoint = camera()->pointUnderPixel(point, found);
	selectedPoint -= 0.01f*dir; // Small offset to make point clearly visible.
	// Note that "found" is different from (selectedObjectId()>=0) because of the size of the select region.

	if (selectedName() == -1)
		QMessageBox::information(this, "No selection",
								 "No object selected under pixel " + QString::number(point.x()) + "," + QString::number(point.y()));
	else
		QMessageBox::information(this, "Selection",
								 "Spiral number " + QString::number(selectedName()) + " selected under pixel " +
								 QString::number(point.x()) + "," + QString::number(point.y()));
}

void WalkmeshWidget::keyPressEvent(QKeyEvent *event)
{
	/*if(lastKeyPressed == event->key()
			&& (event->key() == Qt::Key_Left
				|| event->key() == Qt::Key_Right
				|| event->key() == Qt::Key_Down
				|| event->key() == Qt::Key_Up)) {
		if(transStep > 100.0f) {
			transStep *= 0.90f; // accelerator
		}
	} else {
		transStep = 360.0f;
	}
	lastKeyPressed = event->key();

	switch(event->key())
	{
	case Qt::Key_Left:
		xTrans += 1.0f/transStep;
		updateGL();
		break;
	case Qt::Key_Right:
		xTrans -= 1.0f/transStep;
		updateGL();
		break;
	case Qt::Key_Down:
		yTrans += 1.0f/transStep;
		updateGL();
		break;
	case Qt::Key_Up:
		yTrans -= 1.0f/transStep;
		updateGL();
		break;
	default:
		QGLViewer::keyPressEvent(event);
		return;
	}*/
	QGLViewer::keyPressEvent(event);
}

void WalkmeshWidget::focusInEvent(QFocusEvent *event)
{
	//grabKeyboard();
	QGLViewer::focusInEvent(event);
}

void WalkmeshWidget::focusOutEvent(QFocusEvent *event)
{
	//releaseKeyboard();
	QGLViewer::focusOutEvent(event);
}

static void qNormalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360 * 16;
	while (angle > 360 * 16)
		angle -= 360 * 16;
}

void WalkmeshWidget::setXRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != xRot) {
		xRot = angle;
		updateGL();
	}
}

void WalkmeshWidget::setYRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != yRot) {
		yRot = angle;
		updateGL();
	}
}

void WalkmeshWidget::setZRotation(int angle)
{
	qNormalizeAngle(angle);
	if (angle != zRot) {
		zRot = angle;
		updateGL();
	}
}

void WalkmeshWidget::setZoom(int zoom)
{
	distance = zoom / 4096.0;
}

void WalkmeshWidget::resetCamera()
{
	distance = 0;
	zRot = yRot = xRot = 0;
	xTrans = yTrans = 0;
	updateGL();
}

void WalkmeshWidget::setCurrentFieldCamera(int camID)
{
	this->camID = camID;
	updatePerspective();
}

void WalkmeshWidget::setSelectedTriangle(int triangle)
{
	_selectedTriangle = triangle;
	updateGL();
}

void WalkmeshWidget::setSelectedDoor(int door)
{
	_selectedDoor = door;
	updateGL();
}

void WalkmeshWidget::setSelectedGate(int gate)
{
	_selectedGate = gate;
	updateGL();
}

void WalkmeshWidget::setSelectedArrow(int arrow)
{
	_selectedArrow = arrow;
	updateGL();
}
