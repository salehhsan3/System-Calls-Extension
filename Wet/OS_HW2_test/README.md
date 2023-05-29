# How to use

Copy and paste the files to your VM.

run the following commands:
1. ```chmod +x run_test.sh```
2. ```./run_test.sh```

if you decide to run the get_ancestor_sum_check_init() test, you will have to add the sys_change_weight_of_init() syscall whose implementation is provided at the beginning of 
the hw2_test.cxx document. if you add this new syscall to your machine to pass the tests. make sure you remove it from the relevant files afterwards as it may cause problems
with your submission, make sure to check everything still works after you remove the syscall.
