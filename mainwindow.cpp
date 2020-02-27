#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
*  x Außerdem: Gezeichnet wird ganz gut, aber ist es nach Rechts begrenzt?
*  + Wie in den System Tray bekommen?
*  + Tray Icons und Icons einbauen
*  + Wochentage (Namen) in Timeline anzeigen
*  + Außerdem: Zahlen werden mehrmals übereinander gezeichnet. Das ist hässlich. Wie fixen?
*  - Help fertigmachen
*  - Herunterfahren enablen
*  + Wenn Programm hochgefahren wird während schon triggert: Nicht herunterfahren
*
*  V 1.1:
*  - Großes Problem wenn man das Maximum auf 23:59 stellt, stellt es sich auf 00:00 nachdem man gespeichert hat und neu startet. 1399 -> 1400 min ?
*    Auf den Grund gehen!!!
*  - Shutdown(upper) kann größer als 0 Uhr bzw kleiner als Shutdown(lower) -> Die Zeit vom nächsten Tag wird entsprechend angepasst
*       -> Grenzen entfernen
*       ->
*  - Passwort für Grenzen einbauen -> "Kinderüberwachung"
*  - Bei Systemstart auch Programm starten
*       -> Und dann sofort in den SysTray
*  - Change tab- order (wenn man auf Tag drückt -> Fokus der Elemente ordnen)
*  ? Shutdown- Stop- Window erstellen: Beim triggern erscheint ein Fenster das wartet, bis der User alles bestätigt, damit er alles in Ruhe speichern und herunterfahren kann?
*/

MainWindow::MainWindow(bool *triggered, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    extTriggered(triggered),
    tThread(),
    currentTime(),
    settingsDir(""),
    timeLimitsDir(""),
    // HIER SOLLTE MAN VIELLEICHT DEN PFAD DYNAMISCH ALLOKIEREN
    trayIcon(QIcon("")),
    restoreAction(tr("Restore"), this),
    quitAction(tr("Quit"), this)
{
    ui->setupUi(this);

    try
    {
        tThread.setLooping(true);
        tThread.start();
        tThread.forceTimeUpdate();
    } catch (...)
    {
        // Debug message
    }

    try
    {
        connect(&tThread, SIGNAL(sendTimeUpdate(const std::tm*)),
                this,   SLOT(receiveTimeUpdate(const std::tm*)));

    } catch (...)
    {
        // Debug message
    }

    initializeProgram();

    initializeSystemTrayComponents();

    // Settings Test
    activeSettings.beforeShutdownWarningTime = 60;
    activeSettings.beforeShutdownWarningEnabled = true;
    beforeShutdownWarningAlreadyshown = false;
}

MainWindow::~MainWindow()
{
    delete scene;

    tThread.setLooping(false);
    tThread.quit();
    tThread.exit();
    tThread.terminate();

    delete ui;
}

//==[ Initialization ]=========================================================
void MainWindow::initializeProgram()
{
    on_timeLineButton_released(); // Set default view to timeline
    ui->limitsButtonBox->button(QDialogButtonBox::Ok)->setText("Apply");
    ui->limitsButtonBox->button(QDialogButtonBox::Cancel)->setText("Reset");
    ui->settingsButtonBox->button(QDialogButtonBox::Ok)->setText("Apply");
    ui->settingsButtonBox->button(QDialogButtonBox::Cancel)->setText("Reset");

    // Setting up UI- Pointer- Arrays
    try
    {
        checkBoxes[TimeLimits::Sunday] = ui->sundayCheckBox;
        checkBoxes[TimeLimits::Monday] = ui->mondayCheckBox;
        checkBoxes[TimeLimits::Tuesday] = ui->tuesdayCheckBox;
        checkBoxes[TimeLimits::Wednesday] = ui->wednesdayCheckBox;
        checkBoxes[TimeLimits::Thursday] = ui->thursdayCheckBox;
        checkBoxes[TimeLimits::Friday] = ui->fridayCheckBox;
        checkBoxes[TimeLimits::Saturday] = ui->saturdayCheckBox;
    } catch (std::exception x)
    {
        // Debug message
        qDebug() << "failure: Setting up UI- Pointer- Arrays (checkBoxes)";
    }
    try
    {
        lowerTimeLimitTimeEdits[TimeLimits::Sunday] = ui->sundayLowerLimitEdit;
        lowerTimeLimitTimeEdits[TimeLimits::Monday] = ui->mondayLowerLimitEdit;
        lowerTimeLimitTimeEdits[TimeLimits::Tuesday] = ui->tuesdayLowerLimitEdit;
        lowerTimeLimitTimeEdits[TimeLimits::Wednesday] = ui->wednesdayLowerLimitEdit;
        lowerTimeLimitTimeEdits[TimeLimits::Thursday] = ui->thursdayLowerLimitEdit;
        lowerTimeLimitTimeEdits[TimeLimits::Friday] = ui->fridayLowerLimitEdit;
        lowerTimeLimitTimeEdits[TimeLimits::Saturday] = ui->saturdayLowerLimitEdit;
    } catch (std::exception x)
    {
        // Debug message
        qDebug() << "failure: Setting up UI- Pointer- Arrays (lowerTimeLimitTimeEdits)";
    }
    try
    {
        upperTimeLimitTimeEdits[TimeLimits::Sunday] = ui->sundayUpperLimitEdit;
        upperTimeLimitTimeEdits[TimeLimits::Monday] = ui->mondayUpperLimitEdit;
        upperTimeLimitTimeEdits[TimeLimits::Tuesday] = ui->tuesdayUpperLimitEdit;
        upperTimeLimitTimeEdits[TimeLimits::Wednesday] = ui->wednesdayUpperLimitEdit;
        upperTimeLimitTimeEdits[TimeLimits::Thursday] = ui->thursdayUpperLimitEdit;
        upperTimeLimitTimeEdits[TimeLimits::Friday] = ui->fridayUpperLimitEdit;
        upperTimeLimitTimeEdits[TimeLimits::Saturday] = ui->saturdayUpperLimitEdit;
    } catch (std::exception x)
    {
        // Debug message
        qDebug() << "failure: Setting up UI- Pointer- Arrays (upperTimeLimitTimeEdits)";
    }
    try
    {
        lowerTimeLimitSlider[TimeLimits::Sunday] = ui->sundayLowerLimitSlider;
        lowerTimeLimitSlider[TimeLimits::Monday] = ui->mondayLowerLimitSlider;
        lowerTimeLimitSlider[TimeLimits::Tuesday] = ui->tuesdayLowerLimitSlider;
        lowerTimeLimitSlider[TimeLimits::Wednesday] = ui->wednesdayLowerLimitSlider;
        lowerTimeLimitSlider[TimeLimits::Thursday] = ui->thursdayLowerLimitSlider;
        lowerTimeLimitSlider[TimeLimits::Friday] = ui->fridayLowerLimitSlider;
        lowerTimeLimitSlider[TimeLimits::Saturday] = ui->saturdayLowerLimitSlider;
    } catch (std::exception x)
    {
        // Debug message
        qDebug() << "failure: Setting up UI- Pointer- Arrays (lowerTimeLimitSlider)";
    }
    try
    {
        upperTimeLimitSlider[TimeLimits::Sunday] = ui->sundayUpperLimitSlider;
        upperTimeLimitSlider[TimeLimits::Monday] = ui->mondayUpperLimitSlider;
        upperTimeLimitSlider[TimeLimits::Tuesday] = ui->tuesdayUpperLimitSlider;
        upperTimeLimitSlider[TimeLimits::Wednesday] = ui->wednesdayUpperLimitSlider;
        upperTimeLimitSlider[TimeLimits::Thursday] = ui->thursdayUpperLimitSlider;
        upperTimeLimitSlider[TimeLimits::Friday] = ui->fridayUpperLimitSlider;
        upperTimeLimitSlider[TimeLimits::Saturday] = ui->saturdayUpperLimitSlider;
    } catch (std::exception x)
    {
        // Debug message
        qDebug() << "failure: Setting up UI- Pointer- Arrays (upperTimeLimitSlider)";
    }

    currentPath = QDir::currentPath();

    loadTimeLimits();
}

void MainWindow::initializeTimeLine()
{
    // Smart pointer verwenden!!!
    // Links gibt es eine Linie die nicht angezeigt wird beim ersten Einschalten der
    scene = new QGraphicsScene(this);
    ui->drawView->setScene(scene);

    blackPen = new QPen("black");
    whitePen = new QPen("white");
    grayPen = new QPen("gray");
    redPen = new QPen("red");

    for(int weekDay = TimeLimits::Sunday; weekDay <= TimeLimits::Saturday; weekDay++)
    {
        tlrd[weekDay].activeArea.x = 0;
        tlrd[weekDay].activeArea.y = 0;
        tlrd[weekDay].activeArea.h = 0;
        tlrd[weekDay].activeArea.w = 0;

        tlrd[weekDay].lowerLimits.x = 0;
        tlrd[weekDay].lowerLimits.y = 0;
        tlrd[weekDay].lowerLimits.w = 0;
        tlrd[weekDay].lowerLimits.h = 0;

        tlrd[weekDay].upperLimits.x = 0;
        tlrd[weekDay].upperLimits.y = 0;
        tlrd[weekDay].upperLimits.w = 0;
        tlrd[weekDay].upperLimits.h = 0;
    }

    reinitializeTimeLine();

    // Its okay to initialize this after the timeline was already drawn, because every tick the repaint()- method is called and thus, in a fraction of a time the text gets drawn to the screen
    drawTimeLineTimeText();
}

void MainWindow::reinitializeTimeLine()
{
    int fractionCount = 24; // hours
    drawViewBorderX = ui->drawView->width() - DRAW_WIDTH_OFFSET;
    drawViewBorderY = ui->drawView->height() - DRAW_HEIGHT_OFFSET;

    //qDebug() << "drawViewBorderX = " << drawViewBorderX;
    //qDebug() << "drawViewBorderY = " << drawViewBorderY;

    // Seperator lines of TimeLine
    for(int lineIncrement = 0; lineIncrement < drawViewBorderX; lineIncrement += drawViewBorderX/fractionCount)
    {
        scene->addLine(lineIncrement, 0, lineIncrement, drawViewBorderY, *grayPen);
    }

    //drawTimeLineTimeText();
    determineTimeLineBoxesBoundaries();

    for(int weekDay = TimeLimits::Sunday; weekDay <= TimeLimits::Saturday; weekDay++)
    {
        auto tempRect = scene->addRect(tlrd[weekDay].activeArea.x, tlrd[weekDay].activeArea.y, tlrd[weekDay].activeArea.w, tlrd[weekDay].activeArea.h);
        if(activeTimeLimits.weekdays[weekDay]->isEnabled())
        {
            tempRect->setBrush(QBrush(QColor(51, 102, 255), Qt::SolidPattern));
        }
        else
        {
            tempRect->setBrush(QBrush(QColor(51, 102, 255), Qt::Dense4Pattern));
        }
    }

    /*
    // delete text before redrawing, so that it always stays on top

    drawTimeLineTimeText();
*/
    ui->drawView->repaint();
}

void MainWindow::initializeLimitsView()
{
    // tempTimeLimits = activeTimeLimits
    tempTimeLimits.inhale(activeTimeLimits);

    reinitializeLimitsView();
}

void MainWindow::reinitializeLimitsView()
{
    // Initialize CheckBoxes
    for(int currentDay = TimeLimits::Sunday; currentDay <= TimeLimits::Saturday; currentDay++)
    {
        if(tempTimeLimits.weekdays[currentDay]->isEnabled())
        {
            checkBoxes[currentDay]->setCheckState(Qt::CheckState::Checked);
        }
        else
        {
            checkBoxes[currentDay]->setCheckState(Qt::CheckState::Unchecked);
        }
    }

    determineHeaderCheckBoxState();

    // Initialize lower- & upperTimeLimitEnable
    if(tempTimeLimits.lowerTimeLimitsEnabled)
    {
        ui->lowerLimitsCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->lowerLimitsCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }
    determineLowerTimeLimitsState();

    if(tempTimeLimits.upperTimeLimitsEnabled)
    {
        ui->upperLimitsCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->upperLimitsCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }
    determineUpperTimeLimitsState();

    // Initialize timeLimits
    ui->sundayLowerLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.sunday.getLowerTimeLimitHours(), activeTimeLimits.sunday.getLowerTimeLimitMinutes()));
    ui->sundayUpperLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.sunday.getUpperTimeLimitHours(), activeTimeLimits.sunday.getUpperTimeLimitMinutes()));

    ui->mondayLowerLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.monday.getLowerTimeLimitHours(), activeTimeLimits.monday.getLowerTimeLimitMinutes()));
    ui->mondayUpperLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.monday.getUpperTimeLimitHours(), activeTimeLimits.monday.getUpperTimeLimitMinutes()));

    ui->tuesdayLowerLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.tuesday.getLowerTimeLimitHours(), activeTimeLimits.tuesday.getLowerTimeLimitMinutes()));
    ui->tuesdayUpperLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.tuesday.getUpperTimeLimitHours(), activeTimeLimits.tuesday.getUpperTimeLimitMinutes()));

    ui->wednesdayLowerLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.wednesday.getLowerTimeLimitHours(), activeTimeLimits.wednesday.getLowerTimeLimitMinutes()));
    ui->wednesdayUpperLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.wednesday.getUpperTimeLimitHours(), activeTimeLimits.wednesday.getUpperTimeLimitMinutes()));

    ui->thursdayLowerLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.thursday.getLowerTimeLimitHours(), activeTimeLimits.thursday.getLowerTimeLimitMinutes()));
    ui->thursdayUpperLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.thursday.getUpperTimeLimitHours(), activeTimeLimits.thursday.getUpperTimeLimitMinutes()));

    ui->fridayLowerLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.friday.getLowerTimeLimitHours(), activeTimeLimits.friday.getLowerTimeLimitMinutes()));
    ui->fridayUpperLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.friday.getUpperTimeLimitHours(), activeTimeLimits.friday.getUpperTimeLimitMinutes()));

    ui->saturdayLowerLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.saturday.getLowerTimeLimitHours(), activeTimeLimits.saturday.getLowerTimeLimitMinutes()));
    ui->saturdayUpperLimitSlider->setValue(hours_and_minutes_to_raw(activeTimeLimits.saturday.getUpperTimeLimitHours(), activeTimeLimits.saturday.getUpperTimeLimitMinutes()));

    if(tempTimeLimits.beforeShutdownWarningEnabled)
    {
        ui->warningCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->warningCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }

    ui->warningTimeEdit->setTime(QTime(0, tempTimeLimits.beforeShutdownWarningTime));
}

void MainWindow::initializeSettings()
{   
    reinitializeSettings();
}

void MainWindow::reinitializeSettings()
{
    ui->settingsListWidget->clear();

    ui->settingsListWidget->addItem("General");
    ui->settingsListWidget->addItem("Timeline");
    ui->settingsListWidget->addItem("Limits");

    ui->settingsStackedWidget->setCurrentIndex(0);
}

void MainWindow::initializeHelp()
{
    ui->helpListWidget->clear();

    ui->helpListWidget->addItem("General");
    ui->helpListWidget->addItem("Timeline");
    ui->helpListWidget->addItem("Limits");
    ui->helpListWidget->addItem("About");

    ui->helpStackedWidget->setCurrentIndex(0);
}

//==[ UI Elements ]============================================================

//--( Main Elements )----------------------------------------------------------
void MainWindow::on_timeLineButton_released()
{
    initializeTimeLine();

    ui->stackedWidget->setCurrentIndex(0);

    ui->timeLineButton->setEnabled(false);
    ui->limitsButton->setEnabled(true);
    //ui->settingsButton->setEnabled(true);
    ui->helpButton->setEnabled(true);
}

void MainWindow::on_limitsButton_released()
{
    // Reinitialize tempTimeLimits && limitsView
    initializeLimitsView();

    ui->stackedWidget->setCurrentIndex(1);

    ui->timeLineButton->setEnabled(true);
    ui->limitsButton->setEnabled(false);
    //ui->settingsButton->setEnabled(true);
    ui->helpButton->setEnabled(true);
}

/*
void MainWindow::on_settingsButton_released()
{
    initializeSettings();

    ui->stackedWidget->setCurrentIndex(2);

    ui->timeLineButton->setEnabled(true);
    ui->limitsButton->setEnabled(true);
    ui->settingsButton->setEnabled(false);
    ui->helpButton->setEnabled(true);
}
*/

void MainWindow::on_helpButton_released()
{
    initializeHelp();

    ui->stackedWidget->setCurrentIndex(3);

    ui->timeLineButton->setEnabled(true);
    ui->limitsButton->setEnabled(true);
    //ui->settingsButton->setEnabled(true);
    ui->helpButton->setEnabled(false);
}

//--( Limit Elements) ---------------------------------------------------------
void MainWindow::on_limitsButtonBox_accepted()
{
    // Apply changes
    fetchCheckBoxStates();
    fetchTimeLimits();
    beforeShutdownWarningAlreadyshown = false;
    activeTimeLimits.inhale(tempTimeLimits);
    saveTimeLimits();
}

void MainWindow::on_limitsButtonBox_rejected()
{
    // Reset
    initializeLimitsView();
}

// HeadCheckBox
void MainWindow::on_allHeaderCheckBox_released()
{
    headerCheckBoxAction();
}

//--< Controls listed by day >-------------------------------------------------

// Sunday
void MainWindow::on_sundayCheckBox_released()
{
    determineHeaderCheckBoxState();
}

void MainWindow::on_sundayLowerLimitSlider_valueChanged(int value)
{
    lowerTimeLimitSliderValueChangedAction(TimeLimits::Sunday);
}

void MainWindow::on_sundayLowerLimitEdit_editingFinished()
{
    QTime setTime = ui->sundayLowerLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->sundayLowerLimitSlider->setValue(time);
}

void MainWindow::on_sundayUpperLimitSlider_valueChanged(int value)
{
    upperTimeLimitSliderValueChangedAction(TimeLimits::Sunday);
}

void MainWindow::on_sundayUpperLimitEdit_editingFinished()
{
    QTime setTime = ui->sundayUpperLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->sundayUpperLimitSlider->setValue(time);
}

// Monday
void MainWindow::on_mondayCheckBox_released()
{
    determineHeaderCheckBoxState();
}

void MainWindow::on_mondayLowerLimitSlider_valueChanged(int value)
{
    lowerTimeLimitSliderValueChangedAction(TimeLimits::Monday);
}

void MainWindow::on_mondayLowerLimitEdit_editingFinished()
{
    QTime setTime = ui->mondayLowerLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->mondayLowerLimitSlider->setValue(time);
}

void MainWindow::on_mondayUpperLimitSlider_valueChanged(int value)
{
    upperTimeLimitSliderValueChangedAction(TimeLimits::Monday);
}

void MainWindow::on_mondayUpperLimitEdit_editingFinished()
{
    QTime setTime = ui->mondayUpperLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->mondayUpperLimitSlider->setValue(time);
}

// Tuesday
void MainWindow::on_tuesdayCheckBox_released()
{
    determineHeaderCheckBoxState();
}

void MainWindow::on_tuesdayLowerLimitSlider_valueChanged(int value)
{
    lowerTimeLimitSliderValueChangedAction(TimeLimits::Tuesday);
}

void MainWindow::on_tuesdayLowerLimitEdit_editingFinished()
{
    QTime setTime = ui->tuesdayLowerLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->tuesdayLowerLimitSlider->setValue(time);
}

void MainWindow::on_tuesdayUpperLimitSlider_valueChanged(int value)
{
    upperTimeLimitSliderValueChangedAction(TimeLimits::Tuesday);
}

void MainWindow::on_tuesdayUpperLimitEdit_editingFinished()
{
    QTime setTime = ui->tuesdayUpperLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->tuesdayUpperLimitSlider->setValue(time);
}

// Wednesday
void MainWindow::on_wednesdayCheckBox_released()
{
    determineHeaderCheckBoxState();
}

void MainWindow::on_wednesdayLowerLimitSlider_valueChanged(int value)
{
    lowerTimeLimitSliderValueChangedAction(TimeLimits::Wednesday);
}

void MainWindow::on_wednesdayLowerLimitEdit_editingFinished()
{
    QTime setTime = ui->wednesdayLowerLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->wednesdayLowerLimitSlider->setValue(time);
}

void MainWindow::on_wednesdayUpperLimitSlider_valueChanged(int value)
{
    upperTimeLimitSliderValueChangedAction(TimeLimits::Wednesday);
}

void MainWindow::on_wednesdayUpperLimitEdit_editingFinished()
{
    QTime setTime = ui->wednesdayUpperLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->wednesdayUpperLimitSlider->setValue(time);
}

// Thursday
void MainWindow::on_thursdayCheckBox_released()
{
    determineHeaderCheckBoxState();
}

void MainWindow::on_thursdayLowerLimitSlider_valueChanged(int value)
{
    lowerTimeLimitSliderValueChangedAction(TimeLimits::Thursday);
}

void MainWindow::on_thursdayLowerLimitEdit_editingFinished()
{
    QTime setTime = ui->thursdayLowerLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->thursdayLowerLimitSlider->setValue(time);
}

void MainWindow::on_thursdayUpperLimitSlider_valueChanged(int value)
{
    upperTimeLimitSliderValueChangedAction(TimeLimits::Thursday);
}

void MainWindow::on_thursdayUpperLimitEdit_editingFinished()
{
    QTime setTime = ui->thursdayUpperLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->thursdayUpperLimitSlider->setValue(time);
}

// Friday
void MainWindow::on_fridayCheckBox_released()
{
    determineHeaderCheckBoxState();
}

void MainWindow::on_fridayLowerLimitSlider_valueChanged(int value)
{
    lowerTimeLimitSliderValueChangedAction(TimeLimits::Friday);
}

void MainWindow::on_fridayLowerLimitEdit_editingFinished()
{
    QTime setTime = ui->fridayLowerLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->fridayLowerLimitSlider->setValue(time);
}

void MainWindow::on_fridayUpperLimitSlider_valueChanged(int value)
{
    upperTimeLimitSliderValueChangedAction(TimeLimits::Friday);
}

void MainWindow::on_fridayUpperLimitEdit_editingFinished()
{
    QTime setTime = ui->fridayUpperLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->fridayUpperLimitSlider->setValue(time);
}

// Saturday
void MainWindow::on_saturdayCheckBox_released()
{
    determineHeaderCheckBoxState();
}

void MainWindow::on_saturdayLowerLimitSlider_valueChanged(int value)
{
    lowerTimeLimitSliderValueChangedAction(TimeLimits::Saturday);
}

void MainWindow::on_saturdayLowerLimitEdit_editingFinished()
{
    QTime setTime = ui->saturdayLowerLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->saturdayLowerLimitSlider->setValue(time);
}

void MainWindow::on_saturdayUpperLimitSlider_valueChanged(int value)
{
    upperTimeLimitSliderValueChangedAction(TimeLimits::Saturday);
}

void MainWindow::on_saturdayUpperLimitEdit_editingFinished()
{
    QTime setTime = ui->saturdayUpperLimitEdit->time();

    int time = hours_and_minutes_to_raw(setTime);

    ui->saturdayUpperLimitSlider->setValue(time);
}

//--( Settings Elements)-------------------------------------------------------
/*
 *
 * void MainWindow::on_settingsButtonBox_accepted()
{

}

void MainWindow::on_settingsButtonBox_rejected()
{
    qDebug() << "Q";
}

void MainWindow::on_settingsListWidget_itemClicked(QListWidgetItem *item)
{
    if(item->text().toStdString() == "General")
    {
        ui->settingsStackedWidget->setCurrentIndex(0);
    }
    else if(item->text().toStdString() == "Timeline")
    {
        ui->settingsStackedWidget->setCurrentIndex(1);
    }
    else if(item->text().toStdString() == "Limits")
    {
        ui->settingsStackedWidget->setCurrentIndex(2);
    }
}
*/

//--( Help Elements )----------------------------------------------------------

void MainWindow::on_helpListWidget_itemClicked(QListWidgetItem *item)
{
    if(item->text().toStdString() == "General")
    {
        ui->helpStackedWidget->setCurrentIndex(0);
    }
    else if(item->text().toStdString() == "Timeline")
    {
        ui->helpStackedWidget->setCurrentIndex(1);
    }
    else if(item->text().toStdString() == "Limits")
    {
        ui->helpStackedWidget->setCurrentIndex(2);
    }
    else if(item->text().toStdString() == "About")
    {
        ui->helpStackedWidget->setCurrentIndex(3);
    }
}

void MainWindow::on_walkthroughButton_released()
{
    QString link = "https://sunburst275.jimdofree.com/software/shutdown-monitor/walkthrough/";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_sunburst275Button_released()
{
    QString link = "https://sunburst275.jimdofree.com/";
    QDesktopServices::openUrl(QUrl(link));
}

//--( Timeline Elements )-------------------------------------------------------
void MainWindow::paintEvent(QPaintEvent *e)
{
    int fractionCount = 24 * 60; // hours * minutes (= 1 day)

    x_prev = x_now;
    x_now = int(drawViewBorderX * hours_and_minutes_to_raw(currentTime.tm_hour, currentTime.tm_min)/fractionCount);

    // Initialize timeCursor
    if(timeCursor == nullptr && !timeCursorInitialized)
    {
        timeCursor = scene->addLine(x_now, 0, x_now, drawViewBorderY, *redPen);
        timeCursorInitialized = true;
    }

    // Repaint timeCursor at currentTime, delete old timeCursor
    if(timeCursor != nullptr)
    {
        timeCursor = scene->addLine(x_prev, 0, x_prev, drawViewBorderY, *whitePen);
        timeCursor = scene->addLine(x_now, 0, x_now, drawViewBorderY, *redPen);
    }

    for(int i = 0; i < 24; i++) // 24 because 24 hours a day
    {
        delete timeNumbers[i];
    }
    for(int i = 0; i < TimeLimits::MAX_WEEKDAYS; i++)
    {
        delete weekdayText[i];
    }

    drawTimeLineTimeText();

}

void MainWindow::determineTimeLineBoxesBoundaries()
{
    int fractionCount = 24 * 60; // hours * minutes (= 1 day)

    for(int weekDay = TimeLimits::Sunday; weekDay <= TimeLimits::Saturday; weekDay++)
    {
        int lowerLimitHours = (activeTimeLimits.weekdays[weekDay]->getLowerTimeLimit()).tm_hour;
        int lowerLimitMinutes = (activeTimeLimits.weekdays[weekDay]->getLowerTimeLimit()).tm_min;
        int upperLimitHours = (activeTimeLimits.weekdays[weekDay]->getUpperTimeLimit()).tm_hour;
        int upperLimitMinutes = (activeTimeLimits.weekdays[weekDay]->getUpperTimeLimit()).tm_min;

        //qDebug() << "lowerLimit = " << lowerLimitHours << ":" << lowerLimitMinutes;
        //qDebug() << "upperLimit = " << upperLimitHours << ":" << upperLimitMinutes;

        tlrd[weekDay].activeArea.x = drawViewBorderX * hours_and_minutes_to_raw(lowerLimitHours, lowerLimitMinutes)/fractionCount;
        tlrd[weekDay].activeArea.y = DRAW_RECT_HEIGHT * weekDay;

        // Clip box to right border of timeline screen
        tlrd[weekDay].activeArea.w = drawViewBorderX * (hours_and_minutes_to_raw(upperLimitHours, upperLimitMinutes) - hours_and_minutes_to_raw(lowerLimitHours, lowerLimitMinutes))/fractionCount;
        if((tlrd[weekDay].activeArea.x + tlrd[weekDay].activeArea.w) > drawViewBorderX)
        {
            tlrd[weekDay].activeArea.w = drawViewBorderX - tlrd[weekDay].activeArea.x;
            qDebug() << "Größer";
        }
        tlrd[weekDay].activeArea.h = DRAW_RECT_HEIGHT;
    }
}

void MainWindow::drawTimeLineTimeText()
{

    // Draw time numbers
    int fractionCount = 24; // hours

    timeNumbers[0] = scene->addText("00");
    timeNumbers[1] = scene->addText("01");
    timeNumbers[2] = scene->addText("02");
    timeNumbers[3] = scene->addText("03");
    timeNumbers[4] = scene->addText("04");
    timeNumbers[5] = scene->addText("05");
    timeNumbers[6] = scene->addText("06");
    timeNumbers[7] = scene->addText("07");
    timeNumbers[8] = scene->addText("08");
    timeNumbers[9] = scene->addText("09");
    timeNumbers[10] = scene->addText("10");
    timeNumbers[11] = scene->addText("11");
    timeNumbers[12] = scene->addText("12");
    timeNumbers[13] = scene->addText("13");
    timeNumbers[14] = scene->addText("14");
    timeNumbers[15] = scene->addText("15");
    timeNumbers[16] = scene->addText("16");
    timeNumbers[17] = scene->addText("17");
    timeNumbers[18] = scene->addText("18");
    timeNumbers[19] = scene->addText("19");
    timeNumbers[20] = scene->addText("20");
    timeNumbers[21] = scene->addText("21");
    timeNumbers[22] = scene->addText("22");
    timeNumbers[23] = scene->addText("23");
    int lineIncrement = 0;

    for(int i = 0; i < 24; i++) // 24 because 24 hours a day
    {
        timeNumbers[i]->setPos(lineIncrement + 5, drawViewBorderY - 20); // 20 because otherwise view will start to be scrollable. This is the lowest possible.
        lineIncrement += drawViewBorderX/fractionCount;
    }

    weekdayText[0] = scene->addText("Sunday");
    weekdayText[1] = scene->addText("Monday");
    weekdayText[2] = scene->addText("Tuesday");
    weekdayText[3] = scene->addText("Wednesday");
    weekdayText[4] = scene->addText("Thursday");
    weekdayText[5] = scene->addText("Friday");
    weekdayText[6] = scene->addText("Saturday");

    for(int i = 0; i < TimeLimits::MAX_WEEKDAYS; i++)
    {
        weekdayText[i]->setPos(1, (DRAW_RECT_HEIGHT * i + 5));
        weekdayText[i]->setFont(QFont("Arial", 8, QFont::Bold));
    }
}

//--( UI functions )-----------------------------------------------------------
void MainWindow::fetchCheckBoxStates()
{
    try
    {
        for(int currentDay = TimeLimits::Sunday; currentDay <= TimeLimits::Saturday; currentDay++)
        {
            tempTimeLimits.weekdays[currentDay]->setEnabled(checkBoxes[currentDay]->checkState() == Qt::CheckState::Checked);
        }

        tempTimeLimits.beforeShutdownWarningEnabled = ui->warningCheckBox->checkState() == Qt::CheckState::Checked;
        tempTimeLimits.beforeShutdownWarningTime = hours_and_minutes_to_raw(ui->warningTimeEdit->time());

    } catch (std::exception)
    {
        // Debug message
    }
}

void MainWindow::fetchTimeLimits()
{
    try
    {
        for(int currentDay = TimeLimits::Sunday; currentDay <= TimeLimits::Saturday; currentDay++)
        {
            QTime lowerLimits = lowerTimeLimitTimeEdits[currentDay]->time();
            QTime upperLimits = upperTimeLimitTimeEdits[currentDay]->time();

            tempTimeLimits.weekdays[currentDay]->setLowerTimeLimit(lowerLimits.hour(), lowerLimits.minute());
            tempTimeLimits.weekdays[currentDay]->setUpperTimeLimit(upperLimits.hour(), upperLimits.minute());
        }
    } catch (std::exception)
    {
        // Debug message
    }
}

void MainWindow::determineHeaderCheckBoxState()
{
    int tickedCounter = 0;
    int untickedCounter = 0;

    try
    {
        for(int currentDay = TimeLimits::Sunday; currentDay <= TimeLimits::Saturday; currentDay++)
        {
            bool ticked = checkBoxes[currentDay]->checkState() == Qt::CheckState::Checked;

            tickedCounter += ticked;
            untickedCounter += !ticked;
        }
    } catch (std::exception)
    {
        // Debug message
    }

    //qDebug() << "tickedCounter:\t" << tickedCounter;
    //qDebug() << "untickedCounter:\t" << untickedCounter;

    if( (tickedCounter == 7 && untickedCounter == 0)) // Number of days: 7
    {
        ui->allHeaderCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else if((tickedCounter == 0 && untickedCounter == 7))
    {
        ui->allHeaderCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }
    else
    {
        ui->allHeaderCheckBox->setCheckState(Qt::CheckState::PartiallyChecked);
    }
}

void MainWindow::headerCheckBoxAction()
{
    if(ui->allHeaderCheckBox->checkState() == Qt::CheckState::Checked)
    {
        ui->mondayCheckBox->setCheckState(Qt::CheckState::Checked);
        ui->tuesdayCheckBox->setCheckState(Qt::CheckState::Checked);
        ui->wednesdayCheckBox->setCheckState(Qt::CheckState::Checked);
        ui->thursdayCheckBox->setCheckState(Qt::CheckState::Checked);
        ui->fridayCheckBox->setCheckState(Qt::CheckState::Checked);
        ui->saturdayCheckBox->setCheckState(Qt::CheckState::Checked);
        ui->sundayCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else if (ui->allHeaderCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        ui->mondayCheckBox->setCheckState(Qt::CheckState::Unchecked);
        ui->tuesdayCheckBox->setCheckState(Qt::CheckState::Unchecked);
        ui->wednesdayCheckBox->setCheckState(Qt::CheckState::Unchecked);
        ui->thursdayCheckBox->setCheckState(Qt::CheckState::Unchecked);
        ui->fridayCheckBox->setCheckState(Qt::CheckState::Unchecked);
        ui->saturdayCheckBox->setCheckState(Qt::CheckState::Unchecked);
        ui->sundayCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }

    fetchCheckBoxStates();
}

void MainWindow::lowerTimeLimitSliderValueChangedAction(const unsigned int weekday)
{
    int hours = 0;
    int minutes = 0;

    int lowerTimeLimitSliderValue = getLowerTimeLimitSliderValue(weekday);
    int upperTimeLimitSliderValue = getUpperTimeLimitSliderValue(weekday);

    if(lowerTimeLimitSliderValue >= upperTimeLimitSliderValue)
    {
        std::tie(hours, minutes) = raw_to_hours_and_minutes(upperTimeLimitSliderValue);
        lowerTimeLimitTimeEdits[weekday]->setTime(QTime(hours, minutes));

        lowerTimeLimitSlider[weekday]->setValue(upperTimeLimitSliderValue);
    }
    else
    {
        std::tie(hours, minutes) = raw_to_hours_and_minutes(lowerTimeLimitSliderValue);
        lowerTimeLimitTimeEdits[weekday]->setTime(QTime(hours, minutes));
    }
}

void MainWindow::upperTimeLimitSliderValueChangedAction(const unsigned int weekday)
{
    int hours = 0;
    int minutes = 0;

    int lowerTimeLimitSliderValue = getLowerTimeLimitSliderValue(weekday);
    int upperTimeLimitSliderValue = getUpperTimeLimitSliderValue(weekday);

    if(upperTimeLimitSliderValue <= lowerTimeLimitSliderValue)
    {
        std::tie(hours, minutes)  = raw_to_hours_and_minutes(lowerTimeLimitSliderValue);
        upperTimeLimitTimeEdits[weekday]->setTime(QTime(hours, minutes));

        upperTimeLimitSlider[weekday]->setValue(lowerTimeLimitSliderValue);
    }
    else
    {
        std::tie(hours, minutes)  = raw_to_hours_and_minutes(upperTimeLimitSliderValue);
        upperTimeLimitTimeEdits[weekday]->setTime(QTime(hours, minutes));
    }
}

int MainWindow::getLowerTimeLimitSliderValue(unsigned const int weekday)
{
    if(!(weekday < TimeLimits::Sunday || weekday > TimeLimits::Saturday))
    {
        switch(weekday)
        {
        case TimeLimits::Sunday: return ui->sundayLowerLimitSlider->value();
        case TimeLimits::Monday: return ui->mondayLowerLimitSlider->value();
        case TimeLimits::Tuesday: return ui->tuesdayLowerLimitSlider->value();
        case TimeLimits::Wednesday: return ui->wednesdayLowerLimitSlider->value();
        case TimeLimits::Thursday: return ui->thursdayLowerLimitSlider->value();
        case TimeLimits::Friday: return ui->fridayLowerLimitSlider->value();
        case TimeLimits::Saturday: return ui->saturdayLowerLimitSlider->value();
        }
    }
    return 0;
}

int MainWindow::getUpperTimeLimitSliderValue(unsigned const int weekday)
{
    if(!(weekday < TimeLimits::Sunday || weekday > TimeLimits::Saturday))
    {
        switch(weekday)
        {
        case TimeLimits::Sunday: return ui->sundayUpperLimitSlider->value();
        case TimeLimits::Monday: return ui->mondayUpperLimitSlider->value();
        case TimeLimits::Tuesday: return ui->tuesdayUpperLimitSlider->value();
        case TimeLimits::Wednesday: return ui->wednesdayUpperLimitSlider->value();
        case TimeLimits::Thursday: return ui->thursdayUpperLimitSlider->value();
        case TimeLimits::Friday: return ui->fridayUpperLimitSlider->value();
        case TimeLimits::Saturday: return ui->saturdayUpperLimitSlider->value();
        }
    }
    return 0;
}

void MainWindow::determineLowerTimeLimitsState()
{
    if(ui->lowerLimitsCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempTimeLimits.lowerTimeLimitsEnabled = false;

        for(int weekday = TimeLimits::Sunday; weekday <= TimeLimits::Saturday; weekday++)
        {
            lowerTimeLimitSlider[weekday]->setEnabled(false);
            lowerTimeLimitTimeEdits[weekday]->setEnabled(false);
        }
    }
    else
    {
        tempTimeLimits.lowerTimeLimitsEnabled = true;

        for(int weekday = TimeLimits::Sunday; weekday <= TimeLimits::Saturday; weekday++)
        {
            lowerTimeLimitSlider[weekday]->setEnabled(true);
            lowerTimeLimitTimeEdits[weekday]->setEnabled(true);
        }
    }
}

void MainWindow::determineUpperTimeLimitsState()
{
    if(ui->upperLimitsCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempTimeLimits.upperTimeLimitsEnabled = false;

        for(int weekday = TimeLimits::Sunday; weekday <= TimeLimits::Saturday; weekday++)
        {
            upperTimeLimitSlider[weekday]->setEnabled(false);
            upperTimeLimitTimeEdits[weekday]->setEnabled(false);
        }
    }
    else
    {
        tempTimeLimits.upperTimeLimitsEnabled = true;

        for(int weekday = TimeLimits::Sunday; weekday <= TimeLimits::Saturday; weekday++)
        {
            upperTimeLimitSlider[weekday]->setEnabled(true);
            upperTimeLimitTimeEdits[weekday]->setEnabled(true);
        }
    }
}

void MainWindow::on_lowerLimitsCheckBox_stateChanged(int arg1)
{
    determineLowerTimeLimitsState();
}

void MainWindow::on_upperLimitsCheckBox_stateChanged(int arg1)
{
    determineUpperTimeLimitsState();
}

//==[ Methods ]================================================================

//--( SysTray )----------------------------------------------------------------
void MainWindow::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::WindowStateChange)
    {
            if(isMinimized())
            {
                this->hide();

                trayIcon.show();

                //trayIcon.showMessage(QString("ShutdownMonitor"), QString("ShutdownMonitor runs now in background"), QIcon("D:/Programme/02 Kreatoren/04 Wissenschaft/Qt/Projekte/build-ShutdownMonitor-Desktop_Qt_5_13_0_MinGW_64_bit-Debug/icons//iconicus.ico"));

                event->ignore();
            }

            if(!isMinimized())
            {
                this->show();

                trayIcon.hide();

                event->ignore();
            }
    }
}

void MainWindow::initializeSystemTrayComponents()
{
        // Create trayIcon context menu:
    connect(&restoreAction, &QAction::triggered, this, &MainWindow::on_mainWindow_restoreAction);
    connect(&quitAction, &QAction::triggered, this, &MainWindow::on_mainWindow_quitAction);

    // Create icon path
    std::stringstream iconPath;

    iconPath << currentPath.toStdString();
    iconPath << "/";
    iconPath << trayIconDirName.toStdString();
    iconPath << "/";
    iconPath << trayIconFileName.toStdString();

    // Test whether icon exists
    // This is to make sure that the tray Icon exists and the program can be loaded into the tray and be restored

    QFileInfo fileToCheck(QString::fromStdString(iconPath.str()));
    if(!(fileToCheck.exists() && fileToCheck.isFile()))
    {
        deathMarker = true;
    }

    if(deathMarker)
    {
        std::stringstream errss;
        errss << "Warning: The tray icon seems to be missing.\nPlease make sure it is contained in the following path:\n" << std::endl;
        errss << iconPath.str() << std::endl;

        QMessageBox msgbx;
        msgbx.setText(QString::fromStdString(errss.str()));
        msgbx.setWindowTitle(QString("Warning! Tray Icon Missing"));
        msgbx.setModal(true);
        msgbx.exec();
    }
    else
    {
        trayIcon.setIcon(QIcon(QString::fromStdString(iconPath.str())));
        trayIconMenu.setTitle(QString("Shutdown Monitor"));
        trayIconMenu.addAction(QString::fromStdString("Shutdown Manager"));
        trayIconMenu.addSeparator();
        trayIconMenu.addAction(&restoreAction);
        trayIconMenu.addAction(&quitAction);
        trayIcon.setContextMenu(&trayIconMenu);
    }
}

void MainWindow::on_mainWindow_restoreAction()
{
    this->show();
    this->setWindowState(Qt::WindowState::WindowActive);
}

void MainWindow::on_mainWindow_quitAction()
{
    this->close();
}

//--( Time )-------------------------------------------------------------------
void MainWindow::receiveTimeUpdate(const std::tm *now)
{
    this->currentTime = *now;

    timeUpdate_UI();
    timeWatchdog();
}

void MainWindow::timeUpdate_UI()
{
    std::stringstream outputStringStream;

    // Noch ordentlich formatieren
    switch(currentTime.tm_wday)
    {
    case (TimeLimits::Sunday): outputStringStream << "Sunday";          break;
    case (TimeLimits::Monday): outputStringStream << "Monday";          break;
    case (TimeLimits::Tuesday): outputStringStream << "Tuesday";        break;
    case (TimeLimits::Wednesday): outputStringStream << "Wednesday";    break;
    case (TimeLimits::Thursday): outputStringStream << "Thursday";      break;
    case (TimeLimits::Friday): outputStringStream << "Friday";          break;
    case (TimeLimits::Saturday): outputStringStream << "Saturday";      break;
    }
    outputStringStream << ", ";
    outputStringStream << (currentTime.tm_year + 1900) << "."
                       << std::setfill('0') << std::setw(2) << (currentTime.tm_mon + 1) << "."
                       << std::setfill('0') << std::setw(2) << currentTime.tm_mday;
    outputStringStream << "\n";
    outputStringStream << std::setfill('0') << std::setw(2) << currentTime.tm_hour << ":"
                       << std::setfill('0') << std::setw(2) << currentTime.tm_min << ":"
                       << std::setfill('0') << std::setw(2) << currentTime.tm_sec;

    ui->timeLabel->setText(QString::fromStdString(outputStringStream.str()));

    reinitializeTimeLine();
}

void MainWindow::timeWatchdog()
{
    if(deathMarker)
    {
        this->close();
    }

    bool triggered = false;

    std::tm checkTime = currentTime;
    int weekday_today = activeTimeLimits.weekdays[checkTime.tm_wday]->getWeekday();

    WeekDay *today = activeTimeLimits.weekdays[weekday_today];
    std::tm lowerTime= today->getLowerTimeLimit();
    std::tm upperTime = today->getUpperTimeLimit();

    int rawCheckTime = hours_and_minutes_to_raw(currentTime.tm_hour, currentTime.tm_min);
    int rawUpperTime = hours_and_minutes_to_raw(upperTime.tm_hour, upperTime.tm_min);

    // Debug Test shizzle
    /*{
            std::stringstream tempLowerStream;
            std::stringstream tempCheckStream;
            std::stringstream tempUpperStream;
            std::stringstream tempIncrementStream;

            tempIncrementStream << "--[ " << std::setfill('0') << std::setw(6) << watchDogQDebugOutputIncrementVariable << " ]----------------------------------------------------------------------------";
            tempLowerStream << "Lower Time:   " << lowerTime.tm_wday << " | "
                            << std::setfill('0') << std::setw(2) << lowerTime.tm_hour << ":"
                            << std::setfill('0') << std::setw(2) << lowerTime.tm_min << ":"
                            << std::setfill('0') << std::setw(2) << lowerTime.tm_sec;
            tempCheckStream << "Current Time: " << checkTime.tm_wday << " | "
                            << std::setfill('0') << std::setw(2) << checkTime.tm_hour << ":"
                            << std::setfill('0') << std::setw(2) << checkTime.tm_min << ":"
                            << std::setfill('0') << std::setw(2) << checkTime.tm_sec;
            tempUpperStream << "Upper Time:   " << upperTime.tm_wday << " | "
                            << std::setfill('0') << std::setw(2) << upperTime.tm_hour << ":"
                            << std::setfill('0') << std::setw(2) << upperTime.tm_min << ":"
                            << std::setfill('0') << std::setw(2) << upperTime.tm_sec;

            qDebug() << QString::fromStdString(tempIncrementStream.str());
            qDebug() << QString::fromStdString(tempLowerStream.str());
            qDebug() << QString::fromStdString(tempCheckStream.str());
            qDebug() << QString::fromStdString(tempUpperStream.str());
            qDebug() << "";

            watchDogQDebugOutputIncrementVariable++;
        }*/

    if(today->isEnabled())
    {
        // Lower Time Checking ---------------------------------
        if(activeTimeLimits.lowerTimeLimitsEnabled)
        {
            // Shutdown Check
            if(checkTime.tm_hour < lowerTime.tm_hour)
            {
                triggered = true;
            }
            else if (checkTime.tm_hour == lowerTime.tm_hour && checkTime.tm_min < lowerTime.tm_min) {
                triggered = true;
            }
        }

        // Upper Time Checking ---------------------------------
        if(activeTimeLimits.upperTimeLimitsEnabled)
        {
            // Warning Check
            if(!beforeShutdownWarningAlreadyshown && activeTimeLimits.beforeShutdownWarningEnabled && rawCheckTime >= (rawUpperTime - activeTimeLimits.beforeShutdownWarningTime) && !programStartedOutOfTimeLimits && !startupIteration)
            {
                bool wasMinimized = false;

                // Show warning
                beforeShutdownWarningAlreadyshown = true;
                beforeShutdownWarningAcknowledged = false;

                wasMinimized = isMinimized();

                this->show();
                this->setWindowState(Qt::WindowState::WindowActive);
                trayIcon.hide();

                BeforeShutdownWarningDialog bswd(activeTimeLimits.beforeShutdownWarningTime, &beforeShutdownWarningAcknowledged, this);
                bswd.setModal(true);
                bswd.exec();

                if(wasMinimized)
                {
                    this->hide();
                    this->setWindowState(Qt::WindowState::WindowMinimized);
                    trayIcon.show();
                }
            }

            // Shutdown Check
            if(checkTime.tm_hour > upperTime.tm_hour)
            {
                //qDebug() << "Upper Triggered: h more, min unimportant";
                triggered = true;
            }
            else if(checkTime.tm_hour == upperTime.tm_hour && checkTime.tm_min >= upperTime.tm_min)
            {
               //qDebug() << "Upper Triggered: h equal, min more or equal";
                triggered = true;
            }      
        }

        // Triggered action ------------------------------------
        if(triggered)
        {
            if(!startupIteration && !programStartedOutOfTimeLimits)
            {
                *(this->extTriggered) = true;
                this->close();
            }

            if(startupIteration)
            {
                programStartedOutOfTimeLimits = true;
                triggered = false;
            }

            // Distinguish between :
            //if(settings.instantShutdown) -> Instant shutdown
            //if(!settings.instantShutdown) -> Open Dioalog and wait for user to save and close everything (wait for "OK")
            //(Should I really do this? This kind of undermines the purpose of this program...)
        }
    }

    if(startupIteration && beforeShutdownWarningAcknowledged)
    {
        startupIteration = false;

        this->hide();
        this->setWindowState(Qt::WindowState::WindowMinimized);
        trayIcon.show();
    }

}

std::tuple<int, int> MainWindow::raw_to_hours_and_minutes(const int input_minutes)
{
    int minutes = input_minutes % 60;
    int hours = input_minutes / 60;

    if((hours >= 23 && minutes >= 60) || (hours >= 24 && minutes == 0))
    {
        hours = 23; minutes = 59;
    }

    return std::make_tuple(hours, minutes);
}

int MainWindow::hours_and_minutes_to_raw(QTime input_time)
{
    return (input_time.hour() * 60 + input_time.minute());
}

int MainWindow::hours_and_minutes_to_raw(int h, int m)
{
    return (h * 60 + m);
}

//==[ Saving/Loading ]=========================================================

MainWindow::SaveState MainWindow::saveSettings()
{
    return MainWindow::SaveState::Success;
}

MainWindow::SaveState MainWindow::saveTimeLimits()
{
    qDebug() << "Saving " << timeLimitsFileName << "...";

    bool errorOccured = false;

    std::ofstream saveStream;
    std::stringstream filePath;

    filePath << currentPath.toStdString();
    filePath << "/";
    filePath << timeLimitsDirName.toStdString();

    timeLimitsDir.setPath(QString::fromStdString(filePath.str()));
    if(!timeLimitsDir.exists())
    {
        try
        {
            timeLimitsDir.mkdir(timeLimitsDir.path());
        } catch (QException dirException)
        {
            // Debug message
            qDebug() << "timeLimitsDir -> Problem opening";
        }
    }

    filePath << "/";
    filePath << timeLimitsFileName.toStdString();

    try
    {
        saveStream.open(filePath.str(), std::ios::out | std::ios::binary | std::ios::trunc);
    } catch (std::ofstream::failure failure)
    {
        // Debug message
        qDebug() << "Error occured during file opening.";
        return MainWindow::SaveState::FileOpenError;
    }


    if(saveStream.good() && saveStream.is_open())
    {
        // Save data
        for(int currentDay = TimeLimits::Sunday; currentDay <= TimeLimits::Saturday; currentDay++)
        {
            try
            {
                saveStream.write(reinterpret_cast<char*>(activeTimeLimits.weekdays[currentDay]), sizeof(WeekDay));
            } catch (...)
            {
                // Debug message
                errorOccured = true;
            }
        }

        // Write lowerTimeLimitsEnabled
        try
        {
            qDebug() << "lowerTimeLimitsEnabled:\t" << activeTimeLimits.lowerTimeLimitsEnabled;
            saveStream.write(reinterpret_cast<char*>(&activeTimeLimits.lowerTimeLimitsEnabled), sizeof(bool));
        } catch (...)
        {
            // Debug message
            errorOccured = true;
        }

        // Write upperTimeLimitsEnabled
        try
        {
            qDebug() << "upperTimeLimitsEnabled:\t" << activeTimeLimits.upperTimeLimitsEnabled;
            saveStream.write(reinterpret_cast<char*>(&activeTimeLimits.upperTimeLimitsEnabled), sizeof(bool));
        } catch (...)
        {
            // Debug message
            errorOccured = true;
        }

        // Write warningMessageSettings
        try
        {
            qDebug() << "beforeShutdownWarningEnabled:\t" << activeTimeLimits.beforeShutdownWarningEnabled;
            saveStream.write(reinterpret_cast<char*>(&activeTimeLimits.beforeShutdownWarningEnabled), sizeof(bool));
        } catch (std::exception)
        {
            // Debug message
            errorOccured = true;
        }

        try
        {
            qDebug() << "beforeShutdownWarningTime:\t" << activeTimeLimits.beforeShutdownWarningTime;
            saveStream.write(reinterpret_cast<char*>(&activeTimeLimits.beforeShutdownWarningTime), sizeof(int));
        } catch (...)
        {
            // Debug message
            errorOccured = true;
        }


        if(errorOccured)
        {
            qDebug() << "Error occured during file writing.";
            return MainWindow::SaveState::Failure;
        }
    }

    if(!errorOccured)
    {
        qDebug() << timeLimitsFileName << "successfully saved.";
    }
    saveStream.close();
    return MainWindow::SaveState::Success;
}

MainWindow::LoadState MainWindow::loadSettings()
{
    return MainWindow::LoadState::Success;
}

MainWindow::LoadState MainWindow::loadTimeLimits()
{
    //qDebug() << "Loading " << timeLimitsFileName << "...";

    bool errorOccured = false;

    std::ifstream loadStream;
    std::stringstream filePath;

    filePath << currentPath.toStdString();
    filePath << "/";
    filePath << timeLimitsDirName.toStdString();

    timeLimitsDir.setPath(QString::fromStdString(filePath.str()));
    if(timeLimitsDir.exists())
    {
        filePath << "/";
        filePath << timeLimitsFileName.toStdString();

        try
        {
            loadStream.open(filePath.str(), std::ios::in | std::ios::binary);
        } catch (std::ifstream::failure failure)
        {
            qDebug() << "Error occured during file opening.";
            return MainWindow::LoadState::FileOpenError;
        }

        if(loadStream.good() && loadStream.is_open())
        {
            for(int currentDay = TimeLimits::Sunday; currentDay <= TimeLimits::Saturday; currentDay++)
            {
                try
                {
                    WeekDay temp;
                    loadStream.read(reinterpret_cast<char*>(&temp), sizeof(WeekDay));

                    tempTimeLimits.weekdays[currentDay]->inhale(temp);

                } catch (...)
                {
                    // Debug message
                    errorOccured = true;
                }
            }

            // Read lowerTimeLimitsEnabled
            try
            {
                loadStream.read(reinterpret_cast<char*>(&tempTimeLimits.lowerTimeLimitsEnabled), sizeof(activeTimeLimits.lowerTimeLimitsEnabled));
            } catch (...)
            {
                // Debug message
                errorOccured = true;
            }

            // Read upperTimeLimitsEnabled
            try
            {
                loadStream.read(reinterpret_cast<char*>(&tempTimeLimits.upperTimeLimitsEnabled), sizeof(activeTimeLimits.upperTimeLimitsEnabled));
            } catch (...)
            {
                // Debug message
                errorOccured = true;
            }

            // Read warningMessageSettings
            try
            {
                loadStream.read(reinterpret_cast<char*>(&tempTimeLimits.beforeShutdownWarningEnabled), sizeof(bool));
            } catch (std::exception)
            {
                // Debug message
                errorOccured = true;
            }

            try
            {
                loadStream.read(reinterpret_cast<char*>(&tempTimeLimits.beforeShutdownWarningTime), sizeof(int));
            } catch (...)
            {
                // Debug message
                errorOccured = true;
            }


        }
        else
        {
            errorOccured = true;
            return MainWindow::LoadState::FileOpenError;
        }
    }
    else
    {
        return  MainWindow::LoadState::FileOpenError;
    }

    activeTimeLimits.inhale(tempTimeLimits);
    return MainWindow::LoadState::Success;
}
