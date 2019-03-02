/*
*-------------------------------------------------------------------
* RangeSlider.h
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
#ifndef __RANGE_SLIDER_H__
#define __RANGE_SLIDER_H__

#include "libutil/util.h"
#include <QtWidgets/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

class RangeSlider : public QWidget
{
Q_OBJECT

public:
RangeSlider(QWidget* aParent = Q_NULLPTR, const int id_=0);

QSize minimumSizeHint() const override;

int GetMinimun() const;
void SetMinimum(int aMinimum);

int GetMaximun() const;
void SetMaximum(int aMaximum);

int GetLowerValue() const;
void SetLowerValue(int aLowerValue);

int GetUpperValue() const;
void SetUpperValue(int aUpperValue);

void SetRange(int aMinimum, int aMaximum);
void SetRange(int aMinimum, int aMaximum, int aLowerValue, int aUpperValue);
void SetValues(int aLowerValue, int aUpperValue);

int  getId() { return id; }
void setActive(const bool f) {
active = f;
}

bool isActive() {
return active;
}

protected:
void paintEvent(QPaintEvent* aEvent) override;
void mousePressEvent(QMouseEvent* aEvent) override;
void mouseMoveEvent(QMouseEvent* aEvent) override;
void mouseReleaseEvent(QMouseEvent* aEvent) override;
void changeEvent(QEvent* aEvent) override;

QRectF firstHandleRect() const;
QRectF secondHandleRect() const;
QRectF handleRect(int aValue) const;

signals:
void lowerValueChanged(int aLowerValue);
void upperValueChanged(int aUpperValue);
void rangeChanged(int aMin, int aMax);

public slots:
void setLowerValue(int aLowerValue);
void setUpperValue(int aUpperValue);
void setMinimum(int aMinimum);
void setMaximum(int aMaximum);

private:
Q_DISABLE_COPY(RangeSlider)
float currentPercentage();
int validWidth() const;

bool active;
int  id;
int mMinimum;
int mMaximum;
int mLowerValue;
int mUpperValue;
bool mFirstHandlePressed;
bool mSecondHandlePressed;
int mInterval;
int mDelta;
QColor mBackgroudColorEnabled;
QColor mBackgroudColorDisabled;
QColor mBackgroudColor;
};

typedef std::shared_ptr<RangeSlider> RangeSliderPtr;
#endif
