// C program to demonstrate use of fork() and pipe() 
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 
  
int main() 
{ 
    // We use two pipes 
    // First pipe to send input string from parent 
    // Second pipe to send concatenated string from child 
  
    int fd1[2];  // Used to store two ends of first pipe 
    int fd2[2];  // Used to store two ends of second pipe 
  
    char fixed_str1[] = "howard.edu"; 
    char fixed_str2[] = "gobison.org";
    char input_str[100]; 
    char second_input[100];
    char concat_str[200];
    pid_t p; 
  
    if (pipe(fd1)==-1) 
    { 
        fprintf(stderr, "Pipe 1 Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) 
    { 
        fprintf(stderr, "Pipe 2 Failed" ); 
        return 1; 
    } 
  
    printf("Input : ");
    if (scanf("%99s", input_str) != 1) {
      return 1;
    }
    p = fork(); 
  
    if (p < 0) 
    { 
        fprintf(stderr, "Fork Failed\n" ); 
        return 1; 
    } 
  
    // Parent process 
    if (p > 0) { 
  
        close(fd1[0]);  // Close reading end of pipes 
        close(fd2[1]);
  
        // Write input string and close writing end of first 
        // pipe. 
        write(fd1[1], input_str, strlen(input_str)+1);
        close(fd1[1]);
  
        // Wait for child to print the concatenated string 
        wait(NULL); 
  
        // Read updated string back from pipe 2
        read(fd2[0], concat_str, sizeof(concat_str));// Close writing end of pipes 
        close(fd2[0]); 

        // Concatenate "gobison.org"
        strcat(concat_str, fixed_str2);
        printf("Output : %s\n", concat_str);
    } 
  
    // child process 
    else{ 
        close(fd1[1]);  // Close writing end of first pipes 
        close(fd2[0]); 
      
        // Read a string using first pipe 
        read(fd1[0], concat_str, sizeof(concat_str));
        close(fd1[0]);
  
        strcat(concat_str, fixed_str1);
        printf("Output : %s\n", concat_str);

        printf("Input : ");
        if (scanf("%99s", second_input) != 1){
          exit(1);
        }
  
        strcat(concat_str, second_input);

        // Close both reading ends 
        write(fd2[1], concat_str, strlen(concat_str)+1);
        close(fd2[1]); 
  
        exit(0); 
    } 
    return 0;
} 