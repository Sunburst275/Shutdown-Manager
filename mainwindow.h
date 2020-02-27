#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt headers
#include "QtGui"
#include "QtCore"
#include "QMainWindow"
#include "QMessageBox"
#include "QWidget"
#include "QFileInfo"

#include "QGraphicsScene"
#include "QGraphicsLineItem"
#include "QPen"
#include "QColor"

#include "QException"
#include "QTime"
#include "QDir"
#include "QSlider"
#include "QTimeEdit"
#include "QCheckBox"
#include "QListWidget"

#include "QGroupBox"
#include "QSystemTrayIcon"
#include "QAction"
#include "QMenu"
#include "QLabel"

// Standard C++ headers
#include <locale>
#include <string>
#include <sstream>
#include <fstream>

// Own headers
#include "settings.h"
#include "timethread.h"
#include "timelimits.h"
#include "beforeshutdownwarningdialog.h"
#include "timelinerectdata.h"

// Based on std::tm

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum class SaveState{Success, Failure, FileOpenError};
    enum class LoadState{Success, Failure, FileOpenError};

public:
    explicit MainWindow(bool *triggered, QWidget *parent = nullptr);
    ~MainWindow();

    //==[ Variables ]==============================================================
private:
    Ui::MainWindow *ui;

    bool deathMarker = false;
    bool startupIteration = true;
    bool programStartedOutOfTimeLimits = false;
    bool *extTriggered;

    TimeThread tThread;

    std::tm currentTime;

    QTimeEdit *lowerTimeLimitTimeEdits[TimeLimits::MAX_WEEKDAYS] = {nullptr};
    QTimeEdit *upperTimeLimitTimeEdits[TimeLimits::MAX_WEEKDAYS] = {nullptr};
    QSlider *lowerTimeLimitSlider[TimeLimits::MAX_WEEKDAYS] = {nullptr};
    QSlider *upperTimeLimitSlider[TimeLimits::MAX_WEEKDAYS] = {nullptr};
    QCheckBox *checkBoxes[TimeLimits::MAX_WEEKDAYS] = {nullptr};

    TimeLimits activeTimeLimits;
    TimeLimits tempTimeLimits;

    Settings activeSettings;
    Settings tempSettings;

    bool beforeShutdownWarningAlreadyshown = false;
    bool beforeShutdownWarningAcknowledged = true;

    // Saving/Loading
    QDir settingsDir;
    QDir timeLimitsDir;

    QString currentPath = "";

    QString settingsDirName = "Settings";
    QString timeLimitsDirName = "TimeData";
    QString trayIconDirName = "icons";

    QString settingsFileName = "settings.sms";
    QString timeLimitsFileName = "time_limits.sml";
    QString trayIconFileName = "icon.ico";

    int watchDogQDebugOutputIncrementVariable = 0;

    //--( Timeline Drawing )-------------------------------------------------------

    static constexpr int DRAW_WIDTH_OFFSET = 1;
    static constexpr int DRAW_HEIGHT_OFFSET = 3;
    static constexpr int DRAW_RECT_HEIGHT = 35;

    int fractionCount = 60 * 24; // min * hours (= 1 day)
    int drawViewBorderX;
    int drawViewBorderY;

    int x_prev;
    int x_now;
    bool timeCursorInitialized;

    QGraphicsScene *scene;
    QPen *blackPen;
    QPen *grayPen;
    QPen *whitePen;
    QPen *redPen;

    QGraphicsRectItem *timeLimitRects_ActiveArea[TimeLimits::MAX_WEEKDAYS];
    QGraphicsRectItem *timeLimitRects_LowerLimits[TimeLimits::MAX_WEEKDAYS];
    QGraphicsRectItem *timeLimitRects_UpperLimits[TimeLimits::MAX_WEEKDAYS];
    TimeLineRectData tlrd[TimeLimits::MAX_WEEKDAYS];

    QGraphicsLineItem *timeCursor;

    QGraphicsTextItem *weekdayText[24];
    QGraphicsTextItem *timeNumbers[24];


    //--( System Tray )------------------------------------------------------------
    QSystemTrayIcon trayIcon;
    QMenu trayIconMenu;

    QLabel trayLabel;

    QAction restoreAction;
    QAction quitAction;

    //==[ Slots ]==================================================================
private slots:
    void receiveTimeUpdate(const std::tm *now);

    void on_timeLineButton_released();
    void on_limitsButton_released();

    void on_limitsButtonBox_accepted();
    void on_limitsButtonBox_rejected();

    /*
    void on_settingsButton_released();
    void on_settingsButtonBox_accepted();
    void on_settingsButtonBox_rejected();
    void on_settingsListWidget_itemClicked(QListWidgetItem *item);
    */

    void on_helpButton_released();
    void on_helpListWidget_itemClicked(QListWidgetItem *item);

    void on_allHeaderCheckBox_released();
    void on_lowerLimitsCheckBox_stateChanged(int arg1);
    void on_upperLimitsCheckBox_stateChanged(int arg1);

    // Sunday
    void on_sundayCheckBox_released();
    void on_sundayLowerLimitSlider_valueChanged(int value);
    void on_sundayLowerLimitEdit_editingFinished();
    void on_sundayUpperLimitSlider_valueChanged(int value);
    void on_sundayUpperLimitEdit_editingFinished();

    // Monday
    void on_mondayCheckBox_released();
    void on_mondayLowerLimitSlider_valueChanged(int value);
    void on_mondayLowerLimitEdit_editingFinished();
    void on_mondayUpperLimitSlider_valueChanged(int value);
    void on_mondayUpperLimitEdit_editingFinished();

    // Tuesday
    void on_tuesdayCheckBox_released();
    void on_tuesdayLowerLimitSlider_valueChanged(int value);
    void on_tuesdayLowerLimitEdit_editingFinished();
    void on_tuesdayUpperLimitSlider_valueChanged(int value);
    void on_tuesdayUpperLimitEdit_editingFinished();

    // Wednesday
    void on_wednesdayCheckBox_released();
    void on_wednesdayLowerLimitSlider_valueChanged(int value);
    void on_wednesdayLowerLimitEdit_editingFinished();
    void on_wednesdayUpperLimitSlider_valueChanged(int value);
    void on_wednesdayUpperLimitEdit_editingFinished();

    // Thursday
    void on_thursdayCheckBox_released();
    void on_thursdayLowerLimitSlider_valueChanged(int value);
    void on_thursdayLowerLimitEdit_editingFinished();
    void on_thursdayUpperLimitSlider_valueChanged(int value);
    void on_thursdayUpperLimitEdit_editingFinished();

    // Friday
    void on_fridayCheckBox_released();
    void on_fridayLowerLimitSlider_valueChanged(int value);
    void on_fridayLowerLimitEdit_editingFinished();
    void on_fridayUpperLimitSlider_valueChanged(int value);
    void on_fridayUpperLimitEdit_editingFinished();

    // Saturday
    void on_saturdayCheckBox_released();
    void on_saturdayLowerLimitSlider_valueChanged(int value);
    void on_saturdayLowerLimitEdit_editingFinished();
    void on_saturdayUpperLimitSlider_valueChanged(int value);
    void on_saturdayUpperLimitEdit_editingFinished();

    //==[ Functions ]==============================================================
    void on_walkthroughButton_released();
    void on_sunburst275Button_released();

    void on_mainWindow_restoreAction();
    void on_mainWindow_quitAction();

private:
    void initializeProgram();

    void initializeTimeLine();
    void reinitializeTimeLine();
    void determineTimeLineBoxesBoundaries();
    void drawTimeLineTimeText();

    void initializeLimitsView();
    void reinitializeLimitsView();

    void initializeSettings();
    void reinitializeSettings();

    void initializeHelp();

    void timeUpdate_UI();
    void timeWatchdog();

    SaveState saveSettings();
    SaveState saveTimeLimits();
    LoadState loadSettings();
    LoadState loadTimeLimits();

    void fetchCheckBoxStates();
    void fetchTimeLimits();
    int getLowerTimeLimitSliderValue(unsigned const int weekday);
    int getUpperTimeLimitSliderValue(unsigned const int weekday);

    void determineHeaderCheckBoxState();
    void headerCheckBoxAction();

    void determineLowerTimeLimitsState();
    void determineUpperTimeLimitsState();

    void lowerTimeLimitSliderValueChangedAction(const unsigned int weekday);
    void upperTimeLimitSliderValueChangedAction(const unsigned int weekday);

    static std::tuple<int, int> raw_to_hours_and_minutes(const int in_minutes);
    static int hours_and_minutes_to_raw(QTime input_time);
    static int hours_and_minutes_to_raw(int h, int m);

    void paintEvent(QPaintEvent *e);

    void changeEvent(QEvent *event);

    void initializeSystemTrayComponents();

    void initializeButtonStyle();
};

#endif // MAINWINDOW_H
