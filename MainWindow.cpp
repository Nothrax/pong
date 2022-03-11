
#include "MainWindow.h"

#include <QPainter>
#include <QKeyEvent>

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {

    setWindowTitle("Pong");

    _paddleSize = HEIGHT / 10;

    _player.setP1(QPoint(3, 0));
    _player.setP2(QPoint(3, _paddleSize));

    _computer.setP1(QPoint(WIDTH - 3, 0));
    _computer.setP2(QPoint(WIDTH - 3, _paddleSize));

    _centerLine.setP1(QPoint(this->width() / 2, 0));
    _centerLine.setP2(QPoint(this->width() / 2, this->height()));

    resetBall();

    _timer = new QTimer(this);

    connect(_timer, SIGNAL(timeout()), this, SLOT(timerTick()));
    _timer->start(10);

    _playerScoreLabel = new QLabel(this);
    _playerScoreLabel->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
    QFont font = _playerScoreLabel->font();
    font.setPointSize(42);
    _playerScoreLabel->setFont(font);

    _computerScoreLabel = new QLabel(this);
    _computerScoreLabel->setFont(font);
    _computerScoreLabel->setStyleSheet("background-color: rgba(255, 255, 255, 0);");

    _stopLabel = new QLabel(this);
    QString temp = tr("<font color='%1'>%2</font>");
    _stopLabel->setText(temp.arg("#FFFFFF", "Press space for pause."));
    _stopLabel->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
    _stopLabel->setAlignment(Qt::AlignCenter);
    font.setPointSize(10);
    _stopLabel->setFont(font);

    this->setStyleSheet("background-color: black");
    _computerPosition = calculatePlayerMovement(_computerPosition, _ball.y());
    this->resize(WIDTH, HEIGHT);
}

MainWindow::~MainWindow() {

}

void MainWindow::paintEvent(QPaintEvent *paint) {
    QPainter painter(this);

    QBrush brush(Qt::white);
    painter.setBrush(brush);
    painter.drawEllipse(_ball);

    QPen centerLine(Qt::yellow);
    centerLine.setWidth(2);
    painter.setPen(centerLine);
    painter.drawLine(_centerLine);

    QPen player(Qt::green);
    player.setWidth(5);
    painter.setPen(player);
    painter.drawLine(_player);

    QPen computer(Qt::red);
    computer.setWidth(5);
    painter.setPen(computer);
    painter.drawLine(_computer);

    QString temp = tr("<font color='%1'>%2</font>");
    _playerScoreLabel->setText(temp.arg("#FFFFFF", QString::number(_playerScore)));
    _computerScoreLabel->setText(temp.arg("#FFFFFF", QString::number(_computerScore)));
}

void MainWindow::timerTick() {
    if (!_isPaused) {
        _computer.setP1(QPoint(this->width() - 3, 0));
        _computer.setP2(QPoint(this->width() - 3, _paddleSize));


        updateBallPosition();
        checkBallPosition();
        updatePlayerPosition();
        updateComputerPosition();
    }

    this->update();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    _stepSize = this->height() / _stepSizeScale;
    _paddleSize = this->height() / 10;
    _playerScoreLabel->setGeometry(this->width() / 4, 0, 50, 100);
    _computerScoreLabel->setGeometry(this->width() - this->width() / 4, 0, 50, 100);
    _stopLabel->setGeometry(this->width() / 2 - 75, this->height() - 50, 150, 50);

    _centerLine.setP1(QPoint(this->width() / 2, 0));
    _centerLine.setP2(QPoint(this->width() / 2, this->height()));
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        _isPaused = !_isPaused;
    }
}

void MainWindow::updateBallPosition() {
    int newX = _ball.x() + _directionVector.x() * _ballDirection.x() * _ballSpeed;
    int newY = _ball.y() + _directionVector.y() * _ballDirection.y() * _ballSpeed;

    _ball.setX(newX);
    _ball.setY(newY);
    QSize size(10, 10);
    _ball.setSize(size);
}

void MainWindow::updatePlayerPosition() {
    ///player movement updated base on mouse
    auto cursorPosDiff = mapFromGlobal(QCursor::pos()).y();

    _playerPosition = calculatePlayerMovement(_playerPosition, cursorPosDiff);

    _player.setP1(QPoint(3, _playerPosition - (_paddleSize / 2)));
    _player.setP2(QPoint(3, _playerPosition + (_paddleSize / 2)));
}

void MainWindow::updateComputerPosition() {

    int ball_position = _ball.y();

    ///update player line
    if (_ballDirection.x() == 1) {
        _computerPosition = calculatePlayerMovement(_computerPosition, ball_position);
    }

    _computer.setP1(QPoint(this->width() - 3, _computerPosition - (_paddleSize / 2)));
    _computer.setP2(QPoint(this->width() - 3, _computerPosition + (_paddleSize / 2)));

}

int MainWindow::calculatePlayerMovement(int oldPosition, int diff) {
    int newPosition = oldPosition;
    int offsetScale = 0;
    if (oldPosition < diff) {
        offsetScale = 1;
    } else if (oldPosition > diff) {
        offsetScale = -1;
    }
    if (abs(oldPosition - diff) < _stepSize) {
        newPosition = diff;
    } else {
        newPosition += _stepSize * offsetScale;
    }

    /// trim position by window size
    if (newPosition < _paddleSize / 2) {
        newPosition = _paddleSize / 2;
    } else if (newPosition > this->height() - _paddleSize / 2) {
        newPosition = this->height() - _paddleSize / 2;
    }
    return newPosition;
}

void MainWindow::checkBallPosition() {
    if (_ball.x() < _ball.height()) {
        changeBallDirectionX();
        _ball.setX(_ball.height());
        if (isScore(_playerPosition)) {
            _computerScore += 1;
            resetBall();
        }
    } else if (_ball.x() > width() - _ball.height()) {
        changeBallDirectionX();
        _ball.setX(width() - _ball.height());
        if (isScore(_computerPosition)) {
            _playerScore += 1;
            resetBall();
        }
    } else if (_ball.y() < _ball.height()) {
        changeBallDirectionY();
        _ball.setY(_ball.height());
    } else if (_ball.y() > height() - _ball.height()) {
        changeBallDirectionY();
        _ball.setY(height() - _ball.height());
    }
}

void MainWindow::changeBallDirectionX() {
    _ballDirection.setX(_ballDirection.x() * -1);

    srand(time(nullptr));

    _directionVector.setY(rand() % 4 + 1);
    _directionVector.setX(rand() % 4 + 1);

}

void MainWindow::changeBallDirectionY() {
    _ballDirection.setY(_ballDirection.y() * -1);

    srand(time(nullptr));

    _directionVector.setY(rand() % 4 + 1);
    _directionVector.setX(rand() % 4 + 1);
}

bool MainWindow::isScore(int position) {
    return !(_ball.y() > position - _paddleSize / 2 && _ball.y() < position + _paddleSize / 2);
}

void MainWindow::resetBall() {
    QSize size(10, 10);
    _ball.setX(WIDTH / 2);
    _ball.setY(HEIGHT / 2);
    _ball.setSize(size);
    _directionVector.setY(rand() % 2 + 1);
    _directionVector.setX(rand() % 2 + 1);
    if (rand() % 2 == 1) {
        _ballDirection.setX(-1);
    } else {
        _ballDirection.setX(1);
    }
    if (rand() % 2 == 1) {
        _ballDirection.setY(-1);
    } else {
        _ballDirection.setY(1);
    }
}

