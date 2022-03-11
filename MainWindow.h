#pragma once

#include <QCoreApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QTimer>
#include <QPainter>
#include <QLabel>

#define WIDTH 800
#define HEIGHT 800

/*namespace Ui {
	class MainWindow;
}*/

class MainWindow : public QMainWindow  {
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow() override;
private:
    int _stepSize;
    int _paddleSize;
    bool _isPaused{false};

    QLine _player;
    QLabel *_playerScoreLabel;
    int _playerScore{0};
    int _playerPosition;

    QLine _computer;
    QLabel *_computerScoreLabel;
    int _computerScore{0};
    int _computerPosition;
    int _stepSizeScale = 200;

    QLine _centerLine;

    QRect _ball;
    QPoint _ballDirection{1,1};
    int _ballSpeed{1};
    QPoint _directionVector{1,1};

    QTimer *_timer;
    QLabel *_stopLabel;

    void updatePlayerPosition();
    void updateComputerPosition();
    void updateBallPosition();
    int calculatePlayerMovement(int oldPosition, int diff);
    void checkBallPosition();
    void changeBallDirectionX();
    void changeBallDirectionY();
    bool isScore(int position);
    void resetBall();

private slots:
    void timerTick();

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent *event) override;



};