/*
*-------------------------------------------------------------------
* test_draw_rect.cpp
* Copyright(c) 2019, The RgbVideoEditor Author (Shunguang Wu).
* All Rights Reserved.
* You may not use this file except in compliance with the License:
*      GNU GENERAL PUBLIC LICENSE (Version 3), 29 June 2007
*
* You may obtain a copy of the License at
*
*     https://www.gnu.org/licenses/gpl-3.0.en.html
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*-------------------------------------------------------------------
*/
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsRectItem>
#include <QtOpenGL/QGLWidget>

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

static qreal rnd(qreal max) { return (qrand() / static_cast<qreal>(RAND_MAX)) * max; }

class View : public QGraphicsView {
public:
View(QGraphicsScene *scene, QWidget *parent = 0) : QGraphicsView(scene, parent) {
setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}
void drawBackground(QPainter *, const QRectF &) {
QColor bg(Qt::blue);
glClearColor(bg.redF(), bg.greenF(), bg.blueF(), 1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
};

void setupScene(QGraphicsScene &s)
{
for (int i = 0; i < 10; i++) {
qreal x = rnd(1), y = rnd(1);
QAbstractGraphicsShapeItem * item = new QGraphicsRectItem(x, y, rnd(1 - x), rnd(1 - y));
item->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
item->setPen(QPen(Qt::red, 0));
item->setBrush(Qt::lightGray);
s.addItem(item);
}
}

int test_draw_rect(int argc, char *argv[])
{
QApplication a(argc, argv);
QGraphicsScene s;
setupScene(s);
View v(&s);
v.fitInView(0, 0, 1, 1);
v.show();
v.setDragMode(QGraphicsView::RubberBandDrag);
v.setRenderHint(QPainter::Antialiasing);
return a.exec();
}
