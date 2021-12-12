#include "fssimplewindow.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <thread>
#include<math.h>
#include "ysglfontdata.h"

void DrawCannonbox(double x, double y, double theta)
{
	const double pi = 3.14159;
	theta = theta * pi / 180;
	glColor3f(0, 0, 1);
	glBegin(GL_QUADS);
	glVertex2d(x, y);
	glVertex2d(x - 1, y);
	glVertex2d(x - 1, y - 1);
	glVertex2d(x, y - 1);
	glEnd();


	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glVertex2d(x, y);
	glVertex2d(x + 2 * cos(theta), y + 2 * sin(theta));



	glEnd();
}
void Drawcanon(double mx, double my, double r)
{
	glColor3f(1, 0, 0);
	glBegin(GL_POLYGON);
	for (int i = 0; i < 64; ++i)
	{
		const double pi = 3.14159;
		double a = (double)i * pi / 32.0;
		double c = cos(a);
		double s = sin(a);

		glVertex2d(mx + c * r, my + s * r);
	}
	glEnd();
}

void DrawTarget(double tx, double ty, double tw, double th)
{
	glColor3ub(255, 0, 0);

	glBegin(GL_QUADS);
	glVertex2d(tx, ty);
	glVertex2d(tx + tw, ty);
	glVertex2d(tx + tw, ty - th);
	glVertex2d(tx, ty - th);
	glEnd();
}
void DrawObstacles(double ox, double oy, double ow, double oh)
{
	glColor3ub(0, 255, 0);

	glBegin(GL_QUADS);
	glVertex2d(ox, oy);
	glVertex2d(ox + ow, oy);
	glVertex2d(ox + ow, oy + oh);
	glVertex2d(ox, oy + oh);
	glEnd();
}
double CheckHitObstacle(
	double missileX, double missileY,
	double targetX, double targetY, double targetW, double targetH)
{
	double relX = missileX - targetX;
	double relY = missileY - targetY;

	if (0 <= relX && relX < targetW && 0 <= relY && relY < targetH)
	{
		return 1;
	}
	return 0;
}
double CheckHitTarget(
	double CannonX, double CannonY,
	double targetX, double targetY, double targetW, double targetH)
{
	double relX = CannonX - targetX;
	double relY = targetY - CannonY;

	if (0 <= relX && relX < targetW && 0 <= relY && relY < targetH)
	{
		return 1;
	}
	return 0;
}
int main(void)
{
	const int nobstacles = 5;
	const double pi = 3.14159;
	const double G = 9.81;
	int num_balls, time_count, trajectory_time_count;
	num_balls = 0;
	time_count = 0;
	double x, y, theta, tstate, tx, ty, tw, th, mx, my, mstate, r, dt, v, vx, vy, path_x, path_y, pathvy;
	x = 5; y = 5; theta = 30; tstate = 1;
	tx = 75; ty = 60; tw = 5; th = 5; r = 0.4;
	mx = x + 2 * cos(theta * pi / 180); my = y + 2 * sin(theta * pi / 180); mstate = 0;
	dt = 0.025; v = 40;


	double ox[nobstacles], oy[nobstacles], ow[nobstacles], oh[nobstacles], ov[nobstacles], ostate[nobstacles], count[nobstacles], trajectoryx[500], trajectoryy[500];
	trajectory_time_count = 0;
	srand(time(nullptr));

	for (int i = 0; i < nobstacles; ++i)
	{
		ostate[i] = 1;
		ox[i] = rand() % 65;
		oy[i] = rand() % 45;
		ow[i] = 8.0 + rand() % 7;
		oh[i] = 8.0 + rand() % 7;
		ov[i] = -5.0 + rand() % 10;
		count[i] = 0;
	}

	FsOpenWindow(0, 0, 800, 600, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTranslated(0, 600, 0);
	glRotated(180, 1, 0, 0);
	glScaled(10, 10, 10);
	for (;;)
	{

		FsPollDevice();

		auto  key = FsInkey();
		if (FSKEY_ESC == key)
		{
			break;
		}

		//Moving

		if (FSKEY_LEFT == key)
		{
			theta += 1;
		}
		else if (FSKEY_RIGHT == key)
		{
			theta -= 1;
		}

		else if (FSKEY_UP == key)
		{
			if (y <= 60.0)
			{
				y += 1.0;
			}

		}
		else if (FSKEY_DOWN == key)
		{
			if (y >= 0.0)
			{
				y -= 1.0;
			}
		}
		if (FSKEY_SPACE == key && 0 == mstate)
		{
			mstate = 1;
			mx = x + 2 * cos(theta * pi / 180);
			my = y + 2 * sin(theta * pi / 180);
			vx = v * cos(theta * pi / 180);
			vy = v * sin(theta * pi / 180);
			pathvy = v * sin(theta * pi / 180);
			num_balls += 1;
		}

		if (0 == mstate)
		{
			trajectory_time_count = 0;
			/*for (int i = 0; i < 500; ++i)
			{
				trajectoryx[i] = 0;
				trajectoryy[i] = 0;
			}*/
			mx = x + 2 * cos(theta * pi / 180);
			my = y + 2 * sin(theta * pi / 180);
			path_x = mx;
			path_y = my;

		}

		if (0 != mstate)
		{

			mx = mx + vx * dt;
			vy = vy - G * dt;
			my = my + vy * dt;


			if (mx > 80.0 || my >= 60.0 || my <= 0 || mx <= 0)
			{
				mstate = 0;

			}
		}
		if (0 != tstate)
		{
			if (ty <= 60.0 && ty >= 5.0)
			{
				ty -= 0.25;
			}
			else
			{
				ty = 60.0;
			}
		}
		else if (0 == tstate)
		{
			mstate = 1;
			if (time_count <= 20)
			{
				glColor3ub(0, 0, 255);
				glRasterPos2d(30, 30);
				YsGlDrawFontBitmap20x28("YOU WIN!!!");
				for (int i = 0; i < 100; i += 2)
				{
					glColor3ub(255, 0, 0);
					glBegin(GL_LINES);
					glVertex2d(tx, ty);
					glVertex2d(rand() % 80, rand() % 60);
					glEnd();
				}
				Drawcanon(tx, ty, (rand() % 100) / 10.0);
			}
			else
			{
				break;
			}
			mx = 0;
			my = 0;
			time_count += 1;
		}
		if (0 != mstate && 0 != tstate)
		{
			if (0 != CheckHitTarget(mx, my, tx, ty, tw, th))
			{
				printf("Hit!!\n");
				tstate = 0;
				mstate = 0;

			}
		}
		for (int i = 0; i < nobstacles; ++i)
		{
			if (0 != ostate[i])
			{
				oy[i] = oy[i] + ov[i] * dt;
				if (oy[i] <= 0 || (oy[i] + oh[i] >= 60.0))
				{
					ov[i] = -ov[i];
				}

			}
			if (0 != mstate && 0 != ostate[i])
			{
				if (0 != CheckHitObstacle(mx, my, ox[i], oy[i], ow[i], oh[i]))
				{
					printf("You Hit an Obstacle!\n");
					ostate[i] = 0;
					mstate = 0;

				}
			}

		}

		if (0 != mstate)
		{

			/*for (int i = 0; i < 500; i++)
			{
				glBegin(GL_POINTS);
				glVertex2f(trajectoryx[i], trajectoryy[i]);
				glEnd();

			}*/
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < trajectory_time_count; i++)
			{

				glVertex2d(trajectoryx[i], trajectoryy[i]);


			}
			glEnd();

			trajectoryx[trajectory_time_count] = mx;
			trajectoryy[trajectory_time_count] = my;
			trajectory_time_count += 1;

		}



		// Rendering >>

		DrawCannonbox(x, y, theta);

		for (int i = 0; i < nobstacles; ++i)
		{
			if (0 != ostate[i])
			{
				DrawObstacles(ox[i], oy[i], ow[i], oh[i]);
			}
		}
		if (0 != tstate)
		{
			DrawTarget(tx, ty, tw, th);
		}

		Drawcanon(mx, my, r);
		FsSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		FsSleep(25);  // 

		//std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
	printf("Score=%d", num_balls);
	return 0;
}
