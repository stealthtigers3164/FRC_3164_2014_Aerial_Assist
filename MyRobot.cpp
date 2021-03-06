#include "WPILib.h"
#include "Timer.h"
#include "DriverStation.h"
#include "RobotDrive.h"

/**
 * This program is based off of the IterativeRobot template provided with WPIlb.
 */ 

class RobotDemo : public IterativeRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick1;
	Joystick stick2;
	Joystick stick3;
	Joystick stick4;
	Victor roller;
	Victor launcher;
	JoystickButton launchButton;
	JoystickButton autoLaunchButton;
	JoystickButton primeButton;
	JoystickButton compressorStartButton;
	JoystickButton compressorStopButton;
	JoystickButton rollerForwardButton;
	JoystickButton rollerBackwardButton;
	JoystickButton rollerLiftButton;
	JoystickButton rollerLowerButton;
	JoystickButton launcherEStop;
	JoystickButton spasmButton;
	JoystickButton phoneLaunchButton;
	JoystickButton phonePrimeButton;
	JoystickButton phoneRollerForwardButton;
	JoystickButton phoneRollerBackwardButton;
	JoystickButton phoneRollerLiftButton;
	JoystickButton phoneRollerLowerButton;
	JoystickButton phoneControlOnButton;
	JoystickButton phoneControlOffButton;
	DigitalInput launcherSwitch;
	DigitalInput compressorSwitch;
	Timer launcherTimer;
	Timer primerTimer;
	Timer straightenRobotTimer;
	Timer autoTimer;
	Timer spasmTimer;
	Relay compressor;
	Solenoid rollerValve1;
	Solenoid rollerValve2;
	AnalogChannel rangeFinder;
	bool launching;
	bool priming;
	bool linedUp;
	double sweetSpot;
	int tolerance;
	bool compressorOn;
	bool phoneToggle;
	
	
	float deadzone(float input){
		if(input<.1&&!input<0){
			input = 0;
		}
		if(input>-.1&&!input>0){
			input = 0;
		}
		return input;
	}
	
	void straightenRobot(){
		
		/* A quick synopsis of how this works, as the code is a bit long and fun to read.
		
		1. Get tolerance of the procedure. This will be explained later.
		2. Turn robot right for .3 seconds, then left for .6 seconds. If
			the lowest distance(in inches) is remembered.
		3. The robot then turns right again, looking for that distance again. The
			tolerance is there to prevent a slightly smaller number from registering
			and causing the program to fail. Lower values mean more accuracy, but a
			lower success rate. Don't pick anything too big, obviously.
		
		*/
		
		tolerance = 1; //in inches. This is used later in the program.

		float lowestValue = 10000; // Big Number!
		float currentValue = getInches();
		myRobot.SetLeftRightMotorOutputs (.2, -.2); //left, right
		straightenRobotTimer.Reset();
		straightenRobotTimer.Start();
		cerr << "while1";
		while(!(straightenRobotTimer.HasPeriodPassed(.5))){ //Find the lowest value.
			currentValue = getInches();
			if(currentValue < lowestValue){
				lowestValue=currentValue;
			}
		}
		myRobot.SetLeftRightMotorOutputs (-.2, .2); //left, right
		straightenRobotTimer.Reset();
		straightenRobotTimer.Start();
		cerr << "while2";
		while(!(straightenRobotTimer.HasPeriodPassed(1))){ //Find the lowest value.
			float currentValue = getInches();
			if(currentValue < lowestValue){
				lowestValue=currentValue;				}
			}
		myRobot.SetLeftRightMotorOutputs (0, 0); //left, right
		
		cerr << "while3";
		
		////Now to line up to near where the lowest value was found. There is a small tolerance,
		////defined above.
		
		myRobot.SetLeftRightMotorOutputs (.2, -.2); //left, right
;
		straightenRobotTimer.Reset();
		straightenRobotTimer.Start();
		cerr << "setTimer";
		linedUp = false;
		cerr << "checkedLinedUp";
		while(!(straightenRobotTimer.HasPeriodPassed(1.1)) || !linedUp==true){ //The program calls failure after the .7 seconds, instead of spinning in circles. 
			float currentValue = getInches();
			if(lowestValue + tolerance >= currentValue){
				myRobot.SetLeftRightMotorOutputs (0, 0); //stop
				linedUp = true;
			}
		}
		myRobot.SetLeftRightMotorOutputs ( 0, 0); //left, right
 //stop thge robot after the 11 seconds if not stopped already.
		
		cerr << "while4";
		//We're done! 
			
	}
	
	void autoLaunch(){
		cerr << "autolaunchStarted ";
		while(autoLaunchButton.Get()){}
		cerr << "straighteningRobot ";
		straightenRobot();
		cerr << "straghtened ";
		//Move to catapult "sweet spot". Default is in here, can be overridden by control station.
		sweetSpot= 84; //inches.
		cerr << "sweetspot_defined ";
		if(getInches() < sweetSpot){ //if closer to target than allowed
			//distance to target is less than best distance, back up
			myRobot.ArcadeDrive(.1, 0, true);
		}else if(getInches() > sweetSpot){
			//distance to target is more than best distance, move forward
			myRobot.ArcadeDrive(.1, 0, true);
		}else{
			//we're at the position, stop the car!
			myRobot.ArcadeDrive(0, 0, true);
		}
		cerr << "launching...";
		launch();
		cerr << "completed";
		
		
		
		
	}
	

	float getInches(){ //this reports distance from target in inches.
		float voltage=rangeFinder.GetVoltage();
		float sensitivity = .1024;
		float inches=voltage*sensitivity*1000;
		return inches;
	}

	
	void primeLauncher(){
		cerr << "primeLauncher ";
		if(!launcherSwitch.Get()){
			priming = true;
			launcher.SetSpeed(-.5);	
		}
	}
	
	void primeEnd(){

		if(launcherSwitch.Get() && priming == true){
			cerr << "primeEnd ";
			priming = false;
			launcher.SetSpeed(0);
		}
	}
	
	void launch(){
		//15 kilograms of resistance on launcher.
		cerr << "launch ";
		priming = false;
		launching = true;
		//primeLauncher();
		launcherTimer.Reset();
		launcherTimer.Start();
		launcher.SetSpeed(-.5);
		cerr << "launchSpeedSet ";

	}
	
	void launchEnd(){

		if(launcherTimer.HasPeriodPassed(1)&& launching== true && priming == false){
			cerr << "launchEnd ";
			launching = false;
			launcher.SetSpeed(0);
		}
	}
	
	void compressorHandler(){
		if(compressorSwitch.Get() == true&&compressorOn==true){
			compressor.Set(Relay::kOn);
		}else{
			compressor.Set(Relay::kOff);

		}
	}
	
	void knockBallIntoLauncher(){
		myRobot.MecanumDrive_Cartesian(-.6, 0, 0);
		spasmTimer.Reset();
		spasmTimer.Start();
		while(!spasmTimer.HasPeriodPassed(.3)){
			myRobot.MecanumDrive_Cartesian(-.6, 0, 0);
			checkTimerEvents();
			displayStats();
		}
	/*	myRobot.MecanumDrive_Cartesian(1, 0, 0);
		
		spasmTimer.Reset();
		spasmTimer.Start();
		while(!spasmTimer.HasPeriodPassed(.15)){
			myRobot.MecanumDrive_Cartesian(1, 0, 0);
			checkTimerEvents();
			displayStats();
		}
		*/
		myRobot.Drive(0,0);
	}
	
	
/*	void compressorOn(Relay::Value direction){
		
	}
	
	void compressorOff(Relay::Value direction){
	}*/
	
	void checkTimerEvents(){
		launchEnd();
		primeEnd();
		compressorHandler();
		

		
	}
	
	void displayStats(){
		SmartDashboard::PutNumber("RangeFinder Volts:", rangeFinder.GetVoltage());
		SmartDashboard::PutBoolean("ChooChoo Switch:",launcherSwitch.Get());
		SmartDashboard::PutNumber("RangeFinder Inches",getInches());
		SmartDashboard::PutBoolean("Compressor Operating:", compressor.Get());
	}

public:
	RobotDemo():
		//everything in this list must be declared in the same order as above.
		//myRobot(3, 4, 1, 2),	// frontLeft, rearLeft, frontRight, rearRight //old config,testbot
		myRobot(2, 4, 1, 3),
		stick1(1),
		stick2(2),
		stick3(3),
		stick4(4),
		roller(5),
		launcher(6),
		launchButton(&stick1, 1),
		autoLaunchButton(&stick1, 3),
		primeButton(&stick1, 2),
		compressorStartButton(&stick3, 8),
		compressorStopButton(&stick3, 7),
		rollerForwardButton(&stick3, 2),
		rollerBackwardButton(&stick3, 4),
		rollerLiftButton(&stick3, 3),
		rollerLowerButton(&stick3, 1),
		launcherEStop(&stick3,6),
		spasmButton(&stick2,2),
		phoneLaunchButton(&stick4, 6),
		phonePrimeButton(&stick4, 5),
		phoneRollerForwardButton(&stick4, 3),
		phoneRollerBackwardButton(&stick4, 4),
		phoneRollerLiftButton(&stick4, 1),
		phoneRollerLowerButton(&stick4, 2),
		phoneControlOnButton(&stick1, 6),
		phoneControlOffButton(&stick1, 7),
		launcherSwitch(2),
		compressorSwitch(1),
		launcherTimer(),
		primerTimer(),
		straightenRobotTimer(),
		autoTimer(),
		compressor(7, Relay::kReverseOnly),
		rollerValve1(1),
		rollerValve2(8),
		rangeFinder(1,2)

	{
		myRobot.SetSafetyEnabled(true);
		myRobot.SetExpiration(0.1);
		this->SetPeriod(0); 	//Set update period to sync with robot control packets (20ms nominal)
	
		myRobot.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);
		myRobot.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);
		launching = false;
		compressorOn = true;
		priming = false;
		phoneToggle = false;

	}
	
/**
 * Robot-wide initialization code should go here.
 * 
 * Use this method for default Robot-wide initialization which will
 * be called when the robot is first powered on.  It will be called exactly 1 time.
 */
void RobotDemo::RobotInit() {

	
}

/**
 * Initialization code for disabled mode should go here.
 * 
 * Use this method for initialization code which will be called each time
 * the robot enters disabled mode. 
 */
void RobotDemo::DisabledInit() {
}

/**
 * Periodic code for disabled mode should go here.
 * 
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in disabled mode.
 */
void RobotDemo::DisabledPeriodic() {
}

/**
 * Initialization code for autonomous mode should go here.
 * 
 * Use this method for initialization code which will be called each time
 * the robot enters autonomous mode.
 */
void RobotDemo::AutonomousInit() {
	//Setup oversampling and averaging for rangefinder. The sensor only polls at 20Hz anyway.
	////////////////////UNCOMMENT FOR SIMPLE AUTO MODE////////////////////////
/*
	myRobot.MecanumDrive_Cartesian(-1,0,0);
	autoTimer.Reset();
	autoTimer.Start();
	while(!autoTimer.HasPeriodPassed(5.0)){
		displayStats();
		myRobot.MecanumDrive_Cartesian(-.4,0,0);//keeps watchdog fed
	}
	myRobot.Drive(0,0);

*/
/////////////////////////UNCOMMENT FOR HIGH GOAL AUTO MODE/////////////////////
	/*
	cerr << "beginAuto ";
	displayStats();
	primeLauncher();
	rollerValve1.Set(false);
	rollerValve2.Set(true);
	
	
	roller.SetSpeed(-1);
	
	autoTimer.Reset();
	autoTimer.Start();
	
	while(!autoTimer.HasPeriodPassed(3.0)){
		myRobot.Drive(0,0); //feed watchdog
		displayStats();
		checkTimerEvents(); //check for timer-based events.
	}
	
	cerr << "1 ";
	
	rollerValve1.Set(true);
	rollerValve2.Set(false);
	
	autoTimer.Reset();
	autoTimer.Start();
	
	while(!autoTimer.HasPeriodPassed(2.0)){
		myRobot.Drive(0,0); //feed watchdog
		displayStats();
		checkTimerEvents(); //check for timer-based events.
	}
	
	cerr << "2 ";
	
	rollerValve1.Set(false);
	rollerValve2.Set(true);
	
	autoTimer.Reset();
	autoTimer.Start();
	
	while(!autoTimer.HasPeriodPassed(1.0)){
		myRobot.Drive(0,0); //feed watchdog
		displayStats();
		checkTimerEvents(); //check for timer-based events.
	}
	roller.SetSpeed(0);
	cerr << "3 ";
	while((getInches() > 116)){  //134 latest sweet spot
				displayStats();
				myRobot.MecanumDrive_Cartesian(-.4, 0, 0);
				checkTimerEvents();
			}
	cerr << "4 ";
	//myRobot.MecanumDrive_Cartesian(0,0,0);
	myRobot.Drive(0,0);
	
	while(!launcherSwitch.Get()){
		myRobot.Drive(0,0); //feed watchdog
		displayStats();
		checkTimerEvents(); //check for timer-based events.
	}
	
	
	cerr << "5 ";
	launch();
	
	while(launching == true){
		myRobot.Drive(0,0); //feed watchdog
		displayStats();
		checkTimerEvents(); //check for timer-based events.
	}
	cerr << "6 ";
	while(getInches() > 30){
		displayStats();
		checkTimerEvents();
		myRobot.MecanumDrive_Cartesian(-.4, 0, 0);
	}
	myRobot.Drive(0,0);
	cerr << "autoComplete ";

//	myRobot.SetInvertedMotor(RobotDrive::kFrontRightMotor, false);
//	myRobot.SetInvertedMotor(RobotDrive::kRearRightMotor, false);

*/

	/////////////////////////UNCOMMENT FOR LOW GOAL AUTO MODE/////////////////////
	cerr << "beginAuto ";
	displayStats();
	rollerValve1.Set(false);
	rollerValve2.Set(true);
	
	roller.SetSpeed(-1);
	
	autoTimer.Reset();
	autoTimer.Start();
		
	while(!autoTimer.HasPeriodPassed(4.0)){
		myRobot.Drive(0,0); //feed watchdog
		displayStats();
		checkTimerEvents(); //check for timer-based events.
	}
	
	roller.SetSpeed(0);
	
	autoTimer.Reset();
	autoTimer.Start();
	
	while(!autoTimer.HasPeriodPassed(3.0)){
		
		myRobot.MecanumDrive_Cartesian(.7, 0, 0);
		displayStats();
		checkTimerEvents();
	}
	
	while(!autoTimer.HasPeriodPassed(1.0)){
		
		myRobot.MecanumDrive_Cartesian(.4, 0, 0);
		displayStats();
		checkTimerEvents();
	}
	
	myRobot.Drive(0,0);
	
	roller.SetSpeed(1);
	
	
	
	
}
/**
 * Periodic code for autonomous mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in autonomous mode.
 */
void RobotDemo::AutonomousPeriodic() {
	////STATS////
	displayStats();
	/////////////
	
	//Feed watchdog
	myRobot.SetLeftRightMotorOutputs(0,0);
	

	
}


void RobotDemo::TeleopInit() {
	
}
/**
 * Periodic code for teleop mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in teleop mode.
 */
void RobotDemo::TeleopPeriodic() {
	/////STATS//////
	displayStats();
	
	
	/////DRIVE CODE///////
	
	//ports 1 and 2 are left, 3 and 4 are right
	//float magnitude=stick1.GetX();
	//float direction= stick1.GetY();
	//The following commented-out lines are for holonomic drive.
	
	float magnitude=0;
	float direction=0;
	float rotation=0;
	
	if(phoneToggle==true){
		magnitude=stick4.GetY();
		direction=-stick4.GetX();
		rotation= stick4.GetTwist();
	}else{
		magnitude=stick1.GetY();
		direction=-stick1.GetX();
		 rotation= stick2.GetX();
	}
	
	
	
	if(rollerLiftButton.Get()||phoneRollerLiftButton.Get()){
		rollerValve1.Set(false);
		rollerValve2.Set(true);
	}
	
	if(rollerLowerButton.Get()||phoneRollerLowerButton.Get()){
		rollerValve1.Set(true);
		rollerValve2.Set(false);
	}
	
	if(compressorStartButton.Get()){
		compressor.Set(Relay::kOn);
		compressorOn=true;
	}
	if(compressorStopButton.Get()){
		compressor.Set(Relay::kOff);
		compressorOn=false;
	}
	
	
	if(rollerForwardButton.Get()||phoneRollerForwardButton.Get()){
		roller.SetSpeed(1); //speeds max out at 1.
	}else if(rollerBackwardButton.Get()||phoneRollerBackwardButton.Get()){
		roller.SetSpeed(-1);
	}else{
		roller.SetSpeed(0);
	}


	if(primeButton.Get()||phonePrimeButton.Get()){
		primeLauncher();
	}
	
	if(launcherEStop.Get()){
		launcher.SetSpeed(0);
		priming = false;
	}
	
	if(spasmButton.Get()){
		knockBallIntoLauncher();
	}
	
	myRobot.MecanumDrive_Cartesian(deadzone(magnitude), deadzone(direction), deadzone(rotation));


	
	
	//////Manual Launch//////////
	if(launchButton.Get()==1||phoneLaunchButton.Get()==1){
		launch();
	}
	//////Auto Launch/////////
	if(autoLaunchButton.Get()==1){
		autoLaunch();
	}
	
	if(phoneControlOnButton.Get()){
		phoneToggle=true;
	}
	if(phoneControlOffButton.Get()){
		phoneToggle=false;
	}
		
	///CHECK FOR EXTERNAL TIMER EVENTS
	checkTimerEvents();
}

/**
 * Initialization code for test mode should go here.
 * 
 * Use this method for initialization code which will be called each time
 * the robot enters test mode.
 */
void RobotDemo::TestInit() {

}

/**
 * Periodic code for test mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in test mode.
 */
void RobotDemo::TestPeriodic() {

	if(compressorStartButton.Get()){
		compressor.Set(Relay::kOn);
		compressorOn=true;
	}
	if(compressorStopButton.Get()){
		compressor.Set(Relay::kOff);
		compressorOn=false;
	}
	displayStats();
	checkTimerEvents();	
}
};

START_ROBOT_CLASS(RobotDemo);
