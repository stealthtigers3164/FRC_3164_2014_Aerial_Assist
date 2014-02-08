#include "WPILib.h"
#include "Timer.h"
#include "RobotDrive.h"

/**
 * This program is based off of the IterativeRobot template provided with WPIlb.
 */ 
class RobotDemo : public IterativeRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick1;
	Joystick stick2;
	Victor roller;
	Victor launcher;
	JoystickButton launchButton;
	JoystickButton compressorStartButton;
	JoystickButton compressorStopButton;
	JoystickButton rollerForwardButton;
	JoystickButton rollerBackwardButton;
	DigitalInput launcherSwitch;
	Timer launcherTimer;
	Compressor compressor;

public:
	RobotDemo():
		//everything in this list must be declared in the same order as above.
		myRobot(2, 1, 4, 3),	// frontLeft, rearLeft, frontRight, rearRight
		stick1(1),
		stick2(2),
		roller(5),
		launcher(6),
		launchButton(&stick1, 1),
		compressorStartButton(&stick1, 11),
		compressorStopButton(&stick1, 12),
		rollerForwardButton(&stick1, 5),
		rollerBackwardButton(&stick1, 3),
		launcherSwitch(1),
		launcherTimer(),
		compressor(1,2)
		
	{
		myRobot.SetExpiration(0.1);
		this->SetPeriod(0); 	//Set update period to sync with robot control packets (20ms nominal)
	
		myRobot.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
		myRobot.SetInvertedMotor(RobotDrive::kRearRightMotor, true);
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
	compressor.Start();
}

/**
 * Periodic code for autonomous mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in autonomous mode.
 */
void RobotDemo::AutonomousPeriodic() {
}

/**
 * Initialization code for teleop mode should go here.
 * 
 * Use this method for initialization code which will be called each time
 * the robot enters teleop mode.
 */
void RobotDemo::TeleopInit() {
	compressor.Start();
}
/**
 * Periodic code for teleop mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in teleop mode.
 */
void RobotDemo::TeleopPeriodic() {
	/////DRIVE CODE///////
	
	//ports 1 and 2 are left, 3 and 4 are right
	float magnitude=stick1.GetX();
	float direction= stick1.GetY();
	float rollerArmAngle= stick1.GetZ();
	float rollerSpeed= stick1.GetRawAxis(3);
	float launcherSpeed= stick1.GetRawAxis(4);
	
	if(compressorStartButton.Get()){
		compressor.Start();
	}
	
	if(compressorStopButton.Get()){
		compressor.Stop();
	}
	
	if(rollerForwardButton.Get()){
		roller.SetSpeed(.5); //speeds max out at 1.
	}else if(rollerBackwardButton.Get()){
		roller.SetSpeed(-.5);
	}else{
		roller.SetSpeed(0);
	}


	myRobot.ArcadeDrive ((magnitude), direction, true); // drive with arcade style
	//myRobot.mecanumDrive_polar(magnitude, direction, rotation);


	//////Roller Code/////////
	//roller.SetSpeed(rollerSpeed*.2); //roller set to 20% speed
	//////////////////////////
	
	
	//////launcher Code/Launch Code//////////
	if(launchButton.Get()==1 && launcherSwitch.Get()==1){
		launcherTimer.Reset();
		launcherTimer.Start();
		launcher.SetSpeed(100);
		if(launcherTimer.HasPeriodPassed(.3)){
			while(!(launchButton.Get())){
			}
			launcher.SetSpeed(0);
		}
		
	}
	
		
	
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
}

};

START_ROBOT_CLASS(RobotDemo);

