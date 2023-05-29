#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cassert>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <random>
#include <cstdlib>

// asmlinkage long sys_change_weight_of_init(int weight)
// {
// 	// add this as a new syscall number 338
// // 338 common  change_weight_of_init   sys_change_weight_of_init
// // asmlinkage long sys_change_weight_of_init(int weight);

//     struct task_struct* curr_task = current;
// 	int depth = 0;
//     while (curr_task->pid != 1)
//     {
//         curr_task = curr_task->real_parent;
// 			depth++;
//     }
//     curr_task->weight = weight;
    
//     return depth;
// }

// int change_weight_of_init(int weight) {
//     long r = syscall(338, weight);
//     return r;
// }

int set_weight(int weight) {
    long r = syscall(334, weight);
    return r;
}

int get_weight() {
    long r = syscall(335);
    return r;
}

int get_ancestor_sum() {
	long r = syscall(336);
    return r;
}

pid_t get_heaviest_descendant() {
	long r = syscall(337);
    return r;
}

static int test_num = 0;

void get_weight_basic_test()
{
	std::cout << "beginning test " << test_num <<std::endl;
	int r = get_weight(); 
	std::cout << "get_weight returned " << r << std::endl;
	assert(r == 0);
	std::cout << "ending test " << test_num++ <<std::endl;
}

void set_weight_test(int weight)
{
	std::cout << "beginning test " << test_num <<std::endl;
	int r = set_weight(-weight);
	std::cout << "set_weight returned " << r << std::endl;
	assert(r == -1);
	
	r = set_weight(weight);
	std::cout << "set_weight set to " << weight << std::endl;
	std::cout << "set_weight returned " << r << std::endl;
	
	r = get_weight(); 
	std::cout << "get_weight returned " << r << std::endl;
	assert(r == weight);
	std::cout << "ending test " << test_num++ <<std::endl;
}

void get_weight_of_child_after_fork_test(int weight)
{	
	std::cout << "beginning test " << test_num <<std::endl;
	int r = set_weight(weight);
	std::cout << "set_weight of parent to " << weight << std::endl;
	std::cout << "set_weight returned (parent) " << r << std::endl;

	pid_t child_pid = fork();
	if (child_pid == 0)
	{
		r = get_weight();
		std::cout << "get_weight returned (chils)" << r << std::endl;
		assert(r == weight); // should be the same weight as its parent!
		exit(0);
	}
	wait(NULL); // wait till child process is done!

	r = get_weight(); // make sure parent's weight wasn't altered
	std::cout << "get_weight returned (parent)" << r << std::endl;
	assert(r == weight);
	std::cout << "ending test " << test_num++ <<std::endl;
}

void get_ancestor_sum_with_weightless_ancestors(int weight)
{
	std::cout << "beginning test " << test_num <<std::endl;
	int r = set_weight(weight);
	std::cout << "set_weight set to " << weight << std::endl;
	std::cout << "set_weight returned " << r << std::endl;

	r = get_ancestor_sum();
	std::cout << "get_ancestor_sum returned " << r << std::endl;
	assert(r == weight);
	std::cout << "ending test " << test_num++ <<std::endl;
}

void get_ancestor_sum_with_depth_1(int weight_parent, int weight_child)
{
	std::cout << "beginning test " << test_num <<std::endl;
	int r = set_weight(weight_parent);
	std::cout << "set_weight set to (parent)" << weight_parent << std::endl;
	std::cout << "set_weight returned (parent)" << r << std::endl;
	pid_t child_pid = fork();
	if (child_pid == 0)
	{
		r = set_weight(weight_child);
		std::cout << "set_weight set to (child)" << weight_child << std::endl;
		std::cout << "set_weight returned (child)" << r << std::endl;

		r = get_ancestor_sum();
		std::cout << "get_ancestor_sum returned " << r << std::endl;
		assert(r == (weight_parent + weight_child) );
		exit(0);
	}
	wait(NULL); // wait till child process is done!

	r = get_ancestor_sum();
	std::cout << "get_ancestor_sum returned " << r << std::endl;
	assert(r == weight_parent);
	std::cout << "ending test " << test_num++ <<std::endl;
}

void get_ancestor_sum_with_depth_2(int weight_parent, int weight_child_1, int weight_child_2)
{
	std::cout << "beginning test " << test_num <<std::endl;
	int r = set_weight(weight_parent);
	std::cout << "set_weight set to (parent)" << weight_parent << std::endl;
	std::cout << "set_weight returned (parent)" << r << std::endl;
	
	if ( fork() == 0 )
	{
		// child_1
		r = set_weight(weight_child_1);
		if( fork() == 0 )
		{
			// child_2
			r = set_weight(weight_child_2);
			std::cout << "set_weight set to (child_2)" << weight_child_2 << std::endl;
			std::cout << "set_weight returned (child_2)" << r << std::endl;

			r = get_ancestor_sum();
			std::cout << "get_ancestor_sum returned " << r << std::endl;
			assert(r == (weight_parent + weight_child_1 + weight_child_2) );
			exit(0);
		}

		wait(NULL); // child_1
		std::cout << "set_weight set to (child_1)" << weight_child_1 << std::endl;
		std::cout << "set_weight returned (child_1)" << r << std::endl;

		r = get_ancestor_sum();
		std::cout << "get_ancestor_sum returned " << r << std::endl;
		assert(r == (weight_parent + weight_child_1) );
		exit(0);
	}

	wait(NULL); //parent
	r = get_ancestor_sum();
	std::cout << "get_ancestor_sum returned " << r << std::endl;
	assert(r == weight_parent);
	std::cout << "ending test " << test_num++ <<std::endl;
}

// void get_ancestor_sum_check_init(int weight, int weight_init)
// {
// 	std::cout << "beginning test " << test_num <<std::endl;
// 	int r = set_weight(weight);
// 	std::cout << "set_weight set to (parent)" << weight << std::endl;
// 	std::cout << "set_weight returned (parent)" << r << std::endl;
	
// 	// implement a syscall that changes the weight of init.
// 	change_weight_of_init(weight_init);

// 	r = get_ancestor_sum();
// 	std::cout << "get_ancestor_sum returned " << r << std::endl;
// 	assert(r == (weight + weight_init) );
// 	std::cout << "ending test " << test_num++ <<std::endl;
// }

void get_heaviest_descendant_no_children_test()
{
	std::cout << "beginning test " << test_num <<std::endl;
	wait(NULL); // if children from previous tests still exist we need to get rid of them
	int r = get_heaviest_descendant();
	std::cout << "get_heaviest_descendant returned " << r << std::endl;
	assert(errno == ECHILD);
	std::cout << "ending test " << test_num++ <<std::endl;
}

void get_heaviest_descendant_1_child()
{
	std::cout << "beginning test " << test_num <<std::endl;
	wait(NULL); // if children from previous tests still exist we need to get rid of them
	pid_t child_pid = fork();
	if( child_pid == 0)
	{
		exit(0); // should still be in children of its parent
	}

	int r = get_heaviest_descendant();
	std::cout << "get_heaviest_descendant returned " << r << std::endl;
	assert(r == child_pid);
	std::cout << "ending test " << test_num++ <<std::endl;
}

void get_heaviest_descendant_2_children(int weight_child_1, int weight_child_2)
{
	// assuming the weights are different
	std::cout << "beginning test " << test_num <<std::endl;
	wait(NULL); // if children from previous tests still exist we need to get rid of them
	int r;
	pid_t child_pid_1 = fork();
	if( child_pid_1 == 0)
	{
		pid_t child_pid_2 = fork();
		if( child_pid_2 == 0)
		{	
			r = set_weight(weight_child_2);
			std::cout << "set_weight set to (child_2)" << weight_child_2 << std::endl;
			std::cout << "set_weight returned (child_2)" << r << std::endl;
			exit(0);
		}
		sleep(1);
		r = get_heaviest_descendant();
		std::cout << "get_heaviest_descendant returned ( in child_1)" << r << std::endl;
		assert(r == child_pid_2);

		r = set_weight(weight_child_1);
		std::cout << "set_weight set to (child_1)" << weight_child_1 << std::endl;
		std::cout << "set_weight returned (child_1)" << r << std::endl;
		exit(0);
	}
	sleep(2);
	r = get_heaviest_descendant();
	std::cout << "get_heaviest_descendant returned " << r << std::endl;
	assert(r == child_pid_1);
	std::cout << "ending test " << test_num++ <<std::endl;
}

void get_heaviest_descendant_2_children_lower_pid(int weight_child)
{
	std::cout << "beginning test " << test_num <<std::endl;
	wait(NULL); // if children from previous tests still exist we need to get rid of them
	pid_t* heaviest_pid_pointer; 
	int SLABSIZE = sysconf(_SC_PAGE_SIZE) - 1;
	heaviest_pid_pointer = (pid_t*)mmap(NULL, SLABSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS , -1, 0);
	*heaviest_pid_pointer = INT32_MAX;
	int r;
	pid_t child_pid_1 = fork();
	if( child_pid_1 == 0)
	{
		pid_t child_1_my_pid = getpid();
		pid_t child_pid_2 = fork();
		if( child_pid_2 == 0)
		{	
			pid_t child_2_my_pid = getpid();
			if (child_2_my_pid < *heaviest_pid_pointer )
			{
				*heaviest_pid_pointer = child_2_my_pid;
			}
			
			r = set_weight(weight_child);
			std::cout << "set_weight set to (child_2)" << weight_child << std::endl;
			std::cout << "set_weight returned (child_2)" << r << std::endl;
			exit(0);
		}
		sleep(2);
		r = get_heaviest_descendant();
		std::cout << "get_heaviest_descendant returned ( in child_1)" << r << ". and the heaviest pid is " << *heaviest_pid_pointer << std::endl;
		assert(r == child_pid_2);

		r = set_weight(weight_child);
		std::cout << "set_weight set to (child_1)" << weight_child << std::endl;
		std::cout << "set_weight returned (child_1)" << r << std::endl;

		if (child_1_my_pid < *heaviest_pid_pointer )
		{
			*heaviest_pid_pointer = child_1_my_pid;
		}
		exit(0);
	}
	sleep(4);
	r = get_heaviest_descendant();
	std::cout << "get_heaviest_descendant returned(parent)" << r << ". and the heaviest pid is " << *heaviest_pid_pointer << std::endl;
	assert(r == *heaviest_pid_pointer);
	munmap(heaviest_pid_pointer,sizeof(*heaviest_pid_pointer));
}


int main() 
{
	int weight = 5, weight_parent = 7, weight_child_1 = 13, weight_child_2 = 4, weight_init = 45;
	int r = 0;
	
	// change_weight_of_init(0); // to reset changes from previous runs

	get_weight_basic_test();

	set_weight_test(weight);

	get_weight_of_child_after_fork_test(weight);

	get_ancestor_sum_with_weightless_ancestors(weight);
	
	get_ancestor_sum_with_depth_1(weight_parent, weight_child_1);

	get_ancestor_sum_with_depth_2(weight_parent, weight_child_1, weight_child_2);

	// get_ancestor_sum_check_init(weight,weight_init); to run this test you have to add a new syscall that's provided at the beginning of this doc.
	/*if you decide to run the test above make sure you you remove the added syscall from your submission as it may cause problems!!!!!!!!!!!!!!!!!!*/

	get_heaviest_descendant_no_children_test();

	get_heaviest_descendant_1_child();

	get_heaviest_descendant_2_children(weight_child_1, weight_child_2);

	get_heaviest_descendant_2_children_lower_pid(weight_child_1); 

	std::cout << "=========SUCCESS=========" << std::endl;
	return 0;
}