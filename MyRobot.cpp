#include "WPILib.h"
#include "Timer.h"
#include "DriverStation.h"
#include "RobotDrive.h"

/**
 * This program is based off of the IterativeRobot template provided with WPIlb.
 */ 

// ***88 inches is the sweet spot!***
class RobotDemo : public IterativeRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick1;
	Joystick stick2;
	Joystick stick3;
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
	DigitalInput launcherSwitch;
	Timer launcherTimer;
	Timer primerTimer;
	Timer straightenRobotTimer;
	Compressor compressor;
	Solenoid rollerValve;
	AnalogChannel rangeFinder;
	bool launching;
	bool priming;
	
	
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
		
		int tolerance = 5; //in inches. This is used later in the program.

		float lowestValue = 10000; // Big Number!
		float currentValue = getInches();
		myRobot.ArcadeDrive (0, .05, true); //Turn right
		straightenRobotTimer.Reset();
		straightenRobotTimer.Start();
		while(!(straightenRobotTimer.HasPeriodPassed(.3))){ //Find the lowest value.
			currentValue = getInches();
			if(currentValue < lowestValue){
				lowestValue=currentValue;
			}
		}
		myRobot.ArcadeDrive(0, .05, true); //turn left, twice as long.
		straightenRobotTimer.Reset();
		straightenRobotTimer.Start();
		while(!(straightenRobotTimer.HasPeriodPassed(.6))){ //Find the lowest value.
			float currentValue = getInches();
			if(currentValue < lowestValue){
				lowestValue=currentValue;				}
			}
		myRobot.ArcadeDrive(0, 0, true); //stop moving
		
		
		
		////Now to line up to near where the lowest value was found. There is a small tolerance,
		////defined above.
		
		myRobot.ArcadeDrive(0, .05, true);
		straightenRobotTimer.Reset();
		straightenRobotTimer.Start();
		bool linedUp = false;
		while(!(straightenRobotTimer.HasPeriodPassed(.7)) || linedUp==true){ //The program calls failure after the .7 seconds, instead of spinning in circles. 
			float currentValue = getInches();
			if(lowestValue + tolerance >= currentValue){
				myRobot.ArcadeDrive(0, 0, true);
				linedUp = true;
			}
		}
		myRobot.ArcadeDrive(0, 0, true); //stop thge robot after the 6 seconds if not stopped already.
		
		//We're done! 
			
	}
	
	void autoLaunch(){
		straightenRobot();
		
		//Move to catapult "sweet spot". Default is in here, can be overridden by control station.
		double sweetSpot= 12*10; //10 feet
		sweetSpot=(SmartDashboard::GetNumber("Sweet Spot:")); //not sure if errors would happen if input was blank.
		
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
		
		launch();
		
		
		
		
		
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

		if(launcherSwitch.Get() && launching == false && priming == true){
			cerr << "primeEnd ";
			priming = false;
			launcher.SetSpeed(0);
		}
	}
	
	void launch(){
		cerr << "launch ";
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
	
	void checkTimerEvents(){
		launchEnd();
		primeEnd();
	}

public:
	RobotDemo():
		//everything in this list must be declared in the same order as above.
		myRobot(2, 1, 4, 3),	// frontLeft, rearLeft, frontRight, rearRight
		stick1(1),
		stick2(2),
		stick3(3),
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
		launcherSwitch(2),
		launcherTimer(),
		primerTimer(),
		straightenRobotTimer(),
		compressor(1,7),
		rollerValve(1),
		rangeFinder(1,2)
	{
		myRobot.SetExpiration(0.1);
		this->SetPeriod(0); 	//Set update period to sync with robot control packets (20ms nominal)
	
		myRobot.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
		myRobot.SetInvertedMotor(RobotDrive::kRearRightMotor, true);
		launching = false;
		priming = false;

	}
	
/**
 * Robot-wide initialization code should go here.
 * 
 * Use this method for default Robot-wide initialization which will
 * be called when the robot is first powered on.  It will be called exactly 1 time.
 */
void RobotDemo::RobotInit() {
	SmartDashboard::PutBoolean("Compressor Enabled:",compressor.Enabled());
	SmartDashboard::PutBoolean("Compressor Active:",compressor.GetPressureSwitchValue());
	SmartDashboard::PutNumber("RangeFinder Volts:", rangeFinder.GetVoltage());
	
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
	int bits=16;
	rangeFinder.SetAverageBits(bits); 
	rangeFinder.GetAverageBits(); 
	rangeFinder.SetOversampleBits(bits); 
	rangeFinder.GetOversampleBits();
	
	compressor.Start(); //Start the compressor.
	
	///Begin Autonomous mode!
	
	autoLaunch();
	
	while(!(getInches() < 12)){
	myRobot.ArcadeDrive (.3, 0, true); 
	}
}
/**
 * Periodic code for autonomous mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in autonomous mode.
 */
void RobotDemo::AutonomousPeriodic() {
	////STATS////
	SmartDashboard::PutBoolean("Compressor Enabled:",compressor.Enabled());
	SmartDashboard::PutBoolean("Compressor Active:",compressor.GetPressureSwitchValue());
	SmartDashboard::PutNumber("RangeFinder Volts:", rangeFinder.GetVoltage());
	/////////////
	
	

	
}


void RobotDemo::TeleopInit() {
	
	//Just in case it isn't already on somehow:
	compressor.Start();
}
/**
 * Periodic code for teleop mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in teleop mode.
 */
void RobotDemo::TeleopPeriodic() {
	/////STATS//////
	SmartDashboard::PutBoolean("Compressor Enabled:",compressor.Enabled());
	SmartDashboard::PutBoolean("Compressor Active:",compressor.GetPressureSwitchValue());
	SmartDashboard::PutNumber("RangeFinder Volts:", rangeFinder.GetVoltage());
	SmartDashboard::PutBoolean("ChooChoo Switch:",launcherSwitch.Get());
	SmartDashboard::PutBoolean("ChooChoo Switch:",launching);
	
	
	/////DRIVE CODE///////
	
	//ports 1 and 2 are left, 3 and 4 are right
	float magnitude=stick1.GetX();
	float direction= stick1.GetY();
	//The following commented-out lines are for holonomic drive.
	//float magnitude=stick1.GetY();
	//float direction= stick1.GetX();
	float rotation= stick2.GetZ();

	
	myRobot.ArcadeDrive ((magnitude), direction, true); // drive with arcade style
	
	if(compressorStartButton.Get()){
		compressor.Start();
	}
	
	if(compressorStopButton.Get()){
		compressor.Stop();
	}
	
	if(rollerLiftButton.Get()){
		rollerValve.Set(true);
	}
	
	if(rollerLowerButton.Get()){
		rollerValve.Set(false);
	}
	
	if(rollerForwardButton.Get()){
		roller.SetSpeed(.5); //speeds max out at 1.
	}else if(rollerBackwardButton.Get()){
		roller.SetSpeed(-.5);
	}else{
		roller.SetSpeed(0);
	}


	if(primeButton.Get()){
		primeLauncher();
	}
	//myRobot.mecanumDrive_polar(magnitude, direction, rotation);


	
	
	//////Manual Launch//////////
	if(launchButton.Get()==1 && launcherSwitch.Get()==1){
		launch();
	}
	//////Auto Launch/////////
	if(autoLaunchButton.Get()==1 && launcherSwitch.Get()==1){
		autoLaunch();
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
	compressor.Start();
}

/**
 * Periodic code for test mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in test mode.
 */
void RobotDemo::TestPeriodic() {
	if(compressorStartButton.Get()){
		compressor.Start();
	}
	
	if(compressorStopButton.Get()){
		compressor.Stop();
	}
	
	SmartDashboard::PutBoolean("Compressor Enabled:",compressor.Enabled());
	SmartDashboard::PutBoolean("Compressor Active:",compressor.GetPressureSwitchValue());
	SmartDashboard::PutNumber("RangeFinder Volts:", rangeFinder.GetVoltage());
	
}



};

START_ROBOT_CLASS(RobotDemo);

