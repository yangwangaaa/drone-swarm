#include "supervisor/Model/supervisor.hpp"

Supervisor::Supervisor(ros::NodeHandle nh): n(nh)
{

}

Supervisor::~Supervisor()
{

}

void Supervisor::getFormation(int amount_of_drones, float distance, float v_shape_angle, char shape)
{
  	ros::ServiceClient client = n.serviceClient<formation_control::Formation>("formation_control");
  	formation_control::Formation srv;
  	srv.request.amount_of_drones = amount_of_drones;
	srv.request.shape_type = shape;
	srv.request.distance = distance;
	srv.request.angle = v_shape_angle;
	float result[amount_of_drones][amount_of_drones];

	if (client.call(srv))
  	{
		std::cout << "Matrix Size: " << srv.response.matrix_size << std::endl;
		int iterator = 0;
		for (int i = 0; i < srv.response.matrix_size; ++i)
			for (int j = 0; j < srv.response.matrix_size; ++j)
			{
				result[i][j] = srv.response.data[iterator];
				iterator++;
			}

		std::cout << "Printing response" << std::endl;
		for (int i = 0; i < srv.response.matrix_size; ++i)
		{
			for (int j = 0; j < srv.response.matrix_size; ++j)
			{
				std::cout << result[i][j] << ", ";
				iterator++;
			}
			std::cout << std::endl;
		}

  	}
  	else
  	{
		ROS_ERROR("Failed to call service formation_control");
		return 1;
  	}
}


int main(int argc, char **argv)
{
	ros::init(argc, argv, "srv_call");
  	ros::NodeHandle n;

	Supervisor sup(n);

	sup.getFormation(5, 5, 20, 'g');
	return 0;
}


