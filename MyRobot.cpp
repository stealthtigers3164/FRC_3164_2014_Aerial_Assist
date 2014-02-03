#include "WPILib.h"
#include "Timer.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The IterativeRobot class is the base of a robot application that will automatically call your
 * Periodic methods for each packet based on the mode.
 */ 
class RobotDemo : public IterativeRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick stick1;
	Joystick stick2;
	Victor roller;
	Victor launcher;
	JoystickButton launchButton;
	DigitalInput launcherSwitch;
	Timer launcherTimer;

public:
	RobotDemo():
		//everything in this list must be declared in the same order as above.
		myRobot(2, 1, 4, 3),	// frontLeft, rearLeft, frontRight, rearRight
		stick1(1),
		stick2(2),
		roller(5),
		launcher(6),
		launchButton(&stick2, 1),
		launcherSwitch(1),
		launcherTimer()
		
	{
		myRobot.SetExpiration(0.1);
		this->SetPeriod(0); 	//Set update period to sync with robot control packets (20ms nominal)
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
	
	//myRobot.mecanumDrive_polar(magnitude, direction, rotation);

	myRobot.ArcadeDrive ((magnitude/1), (direction/4), true); //apply arcade drive settings.  

	//////Roller Code/////////
	roller.SetSpeed(rollerSpeed*.2); //roller set to 20% speed
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
}

/**
 * Periodic code for test mode should go here.
 *
 * Use this method for code which will be called periodically at a regular
 * rate while the robot is in test mode.
 */
void RobotDemo::TestPeriodic() {
}

};

START_ROBOT_CLASS(RobotDemo);

