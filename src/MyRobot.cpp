#include <cstdio>

#include "WPILib.h"
/*
To get analog values other than X and Y, use Joystick.getRawAxis(), numbers as per driver station
0=L X Axis
1=L Y Axis
2=L Trigger
3=R Trigger
4=R X Axis
5=R Y Axis
Buttons use .getRawButton(), start from 1 instead of 0, and run 1 to 10
1=A
2=B
3=X
4=Y
5=LB
6=RB
7=SELECT
8=START
9=Left Stick
10=Right Stick
*/

class MyRobot : public SampleRobot
{
    private:
        int joystickChannel = 0;
        Talon * MotorLeft1;
        Talon * MotorLeft2;
        Talon * MotorRight1;
        Talon * MotorRight2;
        Victor * ArmHinge;
        Victor * LiftWinch;
        Victor * PositioningWinch;
        Victor * Whapper;
        DigitalInput * Limit;
        Joystick * Drive;
        Joystick * Controls;

    public:
        void RobotInit()
        {
            /*Robot initialization function*/
            MotorLeft1 = new Talon(1);
            MotorLeft2 = new Talon(2);
            MotorRight1 = new Talon(3);
            MotorRight2 = new Talon(4);
            ArmHinge = new Victor(8);
            LiftWinch = new Victor(5);
            PositioningWinch = new Victor(6);
            Whapper = new Victor(7);
            Limit = new DigitalInput(0); //Added as an example, DigitalInput class not tested
            //stick = new Joystick(driveChannel);
            Drive = new Joystick(0, 6, 10);//Driving controller
            Controls = new Joystick(1, 6, 10);//Winch and arm controller
        }

        void armTest()
        {
            bool rightBumper= Controls->GetRawButton(6);
            bool leftBumper = Controls->GetRawButton(5);
            bool xButton = Controls->GetRawButton(3);
            bool yButton = Controls->GetRawButton(4);
            bool aButton = Controls->GetRawButton(1);
            bool bButton = Controls->GetRawButton(2);

            float leftTrigger=Drive->GetRawAxis(2);
            float rightTrigger=Drive->GetRawAxis(3);
            double magnitude = .5;

            float armHingeSpeed;
            float liftWinchSpeed;
            float posWinchSpeed;
            float whapperSpeed;

            if (rightBumper)
                armHingeSpeed = magnitude;
            else if (leftBumper)
                armHingeSpeed = -magnitude;
            else
                armHingeSpeed = 0;

            if (xButton)
                liftWinchSpeed = magnitude;
            else if (yButton)
                liftWinchSpeed = -magnitude;
            else
                liftWinchSpeed = 0;

            if (aButton)
                posWinchSpeed = magnitude*2;
            else if (bButton)
                posWinchSpeed = -magnitude*1.5;
            else
                posWinchSpeed = 0;

            if (leftTrigger>=0.5)
                whapperSpeed = magnitude;
            else if (rightTrigger>=0.5)
                whapperSpeed = -magnitude;
            else
                whapperSpeed = 0;

            ArmHinge->Set(armHingeSpeed);
            LiftWinch->Set(liftWinchSpeed);
            PositioningWinch->Set(posWinchSpeed);
            Whapper->Set(whapperSpeed);
        }

        void arcadeDrive()
        {
            float straight=Drive->GetRawAxis(1);
            float turn=Drive->GetRawAxis(4);
            bool turbo=Drive->GetRawButton(5); //Left shoulder button acts as turbo

            //Deadband logic

            if (std::abs(straight)<=0.2)
                straight=0;
            if (std::abs(turn)<=0.2)
                turn=0;
            if (!turbo)
            {
                straight=std::pow(straight,3)*0.5;
                turn=std::pow(turn,3)*0.5;
            }
            else
                std::printf("Turbo!"); //Screams "Turbo!" so we know it's going fast :P

            float right=turn+straight;
            float left=turn-straight;

            if (left>1)
                left=1;
            else if (left<-1)
                left=-1;
            if (right>1)
                right=1;
            else if (right<-1)
                right=-1;
            MotorLeft1->Set(left);
            MotorLeft2->Set(left);
            MotorRight1->Set(right);
            MotorRight2->Set(right);
        }

        void tankDrive()
        {
            float left=Drive->GetRawAxis(1)*-1; //Apparently one side is wired in opposition to the other
            float right=Drive->GetRawAxis(5);
            bool turbo=Drive->GetRawButton(5); //Left shoulder button acts as turbo
            //Inputs from Control controller
            //Inputs on the robot itself

            //Deadband logic
            if (std::abs(right)<=0.2)
                right=0;
            if (std::abs(left)<=0.2)
                left=0;
            if (!turbo) //If turbo is on, it scales from -1 through 1, no slowdown. Turbo off will slow down operation as below
            {
                left=std::pow(left,3)*0.5;
                right=std::pow(right,3)*0.5;
            }
            else
                std::printf("Turbo!"); //Screams "Turbo!" so we know it's going fast :P
            MotorLeft1->Set(left);
            MotorLeft2->Set(left);
            MotorRight1->Set(right);
            MotorRight2->Set(right);
        }

        void Autonomous()
        {
            /*Runs the motors autonomously with tank steering*/
            std::printf("autonomous control mode entered");

            while (IsAutonomous() && IsEnabled())
            {
                float x = -0.5;
                float y = 0.5;

                std::printf("autonomous motor rates MotorLeft1=%f, MotorRight1=%f", x, y);
                MotorLeft1->Set(x);
                MotorLeft2->Set(x);
                MotorRight1->Set(y);
                MotorRight2->Set(y);

                Wait(0.02); // wait for a motor update time
            }
        }

        void OperatorControl()
        {
            while (IsOperatorControl() && IsEnabled())
            {
                armTest();
                arcadeDrive();
                // tankDrive()
                Wait(0.02); // wait for a motor update time
            }
        }

        void Test()
        {
            std::printf("entering test mode");

            //USBCamera * camera = new USBCamera();
            //CameraServer * cServer = new CameraServer();
            //camera->StartCapture();
            //try
            //{
            //    cServer->StartAutomaticCapture(std::static_pointer_cast<USBCamera>(camera));
            //}
            //catch (...)
            //{
            //    camera = nullptr;
            //}

            //while (IsTest())
        }
};

START_ROBOT_CLASS(MyRobot)
