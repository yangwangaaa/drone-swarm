#include "../../include/supervisor/View/mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <cstdlib>
#include <csignal>



MainWindow::MainWindow(QWidget *parent, Supervisor &sup) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    supervisor(sup)
{
    shapeSelection = 'g';
    droneAmount = 3;
    droneDistance = 5.0;
    droneAngle = 15.0;
    droneRandomRange = 0.5;
    simTime = 30;
    simRes = 50;
    movementPattern = STATIC;
    frequency = 0.0;
    x = 0.0;
    y = 0.0;

    // Init UI
    ui->setupUi(this);
    ui->shapeCombo->addItem(tr("Grid"));
    ui->shapeCombo->addItem(tr("V-shape"));
    ui->shapeCombo->addItem(tr("Polygon"));

    ui->testCombo->addItem(tr("Static"));
    ui->testCombo->addItem(tr("Linear"));
    ui->testCombo->addItem(tr("Sinusoidal"));

    ui->amplitudeLabel->setEnabled(false);
    ui->velLabel->setEnabled(false);
    ui->slashLabel->setEnabled(true);
    ui->xLabel->setEnabled(false);
    ui->yLabel->setEnabled(false);
    ui->xSpinbox->setEnabled(false);
    ui->ySpinbox->setEnabled(false);
    ui->freqLabel->setEnabled(false);
    ui->freqSpinbox->setEnabled(false);

    ui->droneAmountSpinBox->setRange(3,20);
    ui->droneAmountLabel->setText(tr("Amount of drones (3-20)"));
    qDebug() << droneAngle;

    ui->applyButton->setStyleSheet("background-color: blue; color: rgb(0, 0, 0)");
    ui->startButton->setStyleSheet("background-color: green; color: rgb(0, 0, 0)");
    ui->stopButton->setStyleSheet("background-color: red; color: rgb(0, 0, 0)");

    ui->centralWidget->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignRight,
            ui->centralWidget->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    ui->splitter->setSizes(QList<int>() << 50 << 200);

//    ui->plot->addGraph();
//    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
//    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);

    // Connect
    connect( ui->shapeCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_shapeCombo_currentTextChanged(QString)));

}

MainWindow::~MainWindow()
{
    system("killall -9 -e python");
    delete ui;
    system("rosnode kill execute");
    system("rosnode kill drone_swarm_sim");
    system("rosnode kill kalman_filter");
    system("rosnode kill View");
    system("rosnode kill p_control");
    system("killall -9 -e roslaunch");
}

void MainWindow::on_shapeCombo_currentTextChanged(const QString &shape)
{
    if (shape.toStdString() == "Grid")
    {
        shapeSelection = 'g';
        ui->droneAngleLabel->setEnabled(false);
        ui->droneAngleSpinbox->setEnabled(false);
        ui->droneAmountSpinBox->setRange(3,20);
        ui->droneAmountLabel->setText(tr("Amount of drones (3-20)"));
    }
    else if (shape.toStdString() == "Polygon")
    {
        shapeSelection = 'p';
        ui->droneAngleLabel->setEnabled(false);
        ui->droneAngleSpinbox->setEnabled(false);
        ui->droneAmountSpinBox->setRange(3,9);
        ui->droneAmountLabel->setText(tr("Amount of drones (3-9)"));
    }
    else if (shape.toStdString() == "V-shape")
    {
        shapeSelection = 'v';
        ui->droneAngleLabel->setEnabled(true);
        ui->droneAngleSpinbox->setEnabled(true);
        ui->droneAmountSpinBox->setRange(3,20);
        ui->droneAmountLabel->setText(tr("Amount of drones (3-20)"));
    }
    std::cout << "Shape selected: " << shapeSelection << std::endl;
}

void MainWindow::on_droneDistanceSpinbox_valueChanged(double distance)
{
    droneDistance = static_cast<float>(distance);
    qDebug() << "Distance: " << droneDistance;
}

void MainWindow::on_droneAngleSpinbox_valueChanged(double angle)
{
    droneAngle = static_cast<float>(angle);
    qDebug() << "Angle: " << droneAngle;
}

void MainWindow::on_droneAmountSpinBox_valueChanged(int amount)
{
    droneAmount = amount;
    qDebug() << "Amount: " << droneAmount;
}

void MainWindow::on_applyButton_clicked()
{
    qDebug() << "Save settings button clicked";
    supervisor.setupSimulation(
                droneAmount, droneDistance, droneAngle, shapeSelection, droneRandomRange,
                simRes, simTime, movementPattern, x, y, frequency
                );
}

void MainWindow::on_startButton_clicked()
{
    qDebug() << "Start button clicked";
    supervisor.startSimulation();
}

void MainWindow::on_stopButton_clicked()
{
    qDebug() << "Stop button clicked";
    supervisor.stopSimulation();
}

void MainWindow::on_initRangeSpinBox_valueChanged(double range)
{
    droneRandomRange = range;
    qDebug() << "Init range: " << droneRandomRange;
}

void MainWindow::on_simTimeSpinbox_valueChanged(double simTime_)
{
    simTime = simTime_;
    qDebug() << "Simulation time: " << simTime << " s";
}

void MainWindow::on_simResSpinbox_valueChanged(int simRes_)
{
    simRes = simRes_;
    qDebug() << "Simulation timestep resolution: " << simRes << " ms";
}

void MainWindow::on_testCombo_currentTextChanged(const QString &pattern)
{
    if (pattern.toStdString() == "Static")
    {
        movementPattern = STATIC;
        ui->amplitudeLabel->setEnabled(false);
        ui->velLabel->setEnabled(false);
        ui->xLabel->setEnabled(false);
        ui->yLabel->setEnabled(false);
        ui->xSpinbox->setEnabled(false);
        ui->ySpinbox->setEnabled(false);
        ui->freqLabel->setEnabled(false);
        ui->freqSpinbox->setEnabled(false);
        ui->amplitudeLabel->setStyleSheet("font-weight: normal");
        ui->velLabel->setStyleSheet("font-weight: normal");
    }
    else if (pattern.toStdString() == "Linear")
    {
        movementPattern = LINEAR;
        ui->amplitudeLabel->setEnabled(false);
        ui->velLabel->setEnabled(true);
        ui->xLabel->setEnabled(true);
        ui->yLabel->setEnabled(true);
        ui->xSpinbox->setEnabled(true);
        ui->ySpinbox->setEnabled(true);
        ui->freqLabel->setEnabled(false);
        ui->freqSpinbox->setEnabled(false);
        ui->amplitudeLabel->setStyleSheet("font-weight: normal");
        ui->velLabel->setStyleSheet("font-weight: bold");
    }
    else if (pattern.toStdString() == "Sinusoidal")
    {
        movementPattern = SINUSOIDAL;
        ui->amplitudeLabel->setEnabled(true);
        ui->velLabel->setEnabled(false);
        ui->xLabel->setEnabled(true);
        ui->yLabel->setEnabled(true);
        ui->xSpinbox->setEnabled(true);
        ui->ySpinbox->setEnabled(true);
        ui->freqLabel->setEnabled(true);
        ui->freqSpinbox->setEnabled(true);
        ui->amplitudeLabel->setStyleSheet("font-weight: bold");
        ui->velLabel->setStyleSheet("font-weight: normal");
    }
    qDebug() << "Movement pattern: " << pattern << "(" << movementPattern << ")";
}

void MainWindow::on_freqSpinbox_valueChanged(double freq)
{
    frequency = freq;
    qDebug() << "Frequency: " << frequency << "Hz";
}

void MainWindow::on_xSpinbox_valueChanged(double xin)
{
    x = xin;
    qDebug() << "x: " << x << ", y: " << y;
}

void MainWindow::on_ySpinbox_valueChanged(double yin)
{
    y = yin;
    qDebug() << "x: " << x << ", y: " << y;
}
