/*
*-------------------------------------------------------------------
* PlayerControls.h
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

#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QtMultimedia/QMediaPlayer>
#include <QtWidgets/QWidget>

class QAbstractButton;
class QAbstractSlider;
class QComboBox;

class PlayerControls : public QWidget
{
Q_OBJECT

public:
PlayerControls(QWidget *parent = 0);

QMediaPlayer::State state() const;
int volume() const;
bool isMuted() const;
qreal playbackRate() const;

public slots:
void setState(QMediaPlayer::State state);
void setVolume(int volume);
void setMuted(bool muted);
void setPlaybackRate(float rate);
signals:
void play();
void pause();
void stop();
void next();
void previous();
void changeVolume(int volume);
void changeMuting(bool muting);
void changeRate(qreal rate);

private slots:
void playClicked();
void muteClicked();
void updateRate();

private:
QMediaPlayer::State playerState;
bool playerMuted;
QAbstractButton *playButton;
QAbstractButton *stopButton;
QAbstractButton *nextButton;
QAbstractButton *previousButton;
QAbstractButton *muteButton;
QAbstractSlider *volumeSlider;
QComboBox *rateBox;
};

#endif // PLAYERCONTROLS_H
