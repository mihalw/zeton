#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#define MSG_TAG 100
#define MSG_SIZE 1

bool resend = false, can_send_token = false;
int color = -1; // -1 to bialy, 1 to czarny
int last_received_token = 1, last_sended_token = 1;
int my_rank, size, confirmation;

void SendToken(int sender, int receiver, int color)
{
	if (can_send_token)
	{
		can_send_token = false;
		resend = true;
		last_sended_token *= -1;
		color = last_sended_token;
		while (resend)
		{
			MPI_Send(&color, MSG_SIZE, MPI_INT, receiver, MSG_TAG, MPI_COMM_WORLD);
			printf("%d: Wyslalem token w kolorze %d do %d\n", my_rank, color, (my_rank + 1) % size);
			resend = false; // potem to usunac
		}
	}
}

int main(int argc, char **argv)
{
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (my_rank == 0)
	{
		can_send_token = true;
		SendToken(my_rank, (my_rank + 1) % size, color);
	}

	for (int i = 0; i < 3; i++)
	{
		MPI_Recv(&color, MSG_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		printf("%d: Otrzymalem token o wartosci %d od %d\n", my_rank, color, status.MPI_SOURCE);
		//MPI_Send(&confirmation, MSG_SIZE, MPI_INT, my_rank-1, MSG_TAG, MPI_COMM_WORLD);

		if (color != last_received_token)
		{
			resend = false;
			last_received_token = color;
			can_send_token = true;
		}
		//MPI_Recv(&confirmation, MSG_SIZE, MPI_INT, my_rank+1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		SendToken(my_rank, (my_rank + 1) % size, color);
	}
	MPI_Finalize();
}
