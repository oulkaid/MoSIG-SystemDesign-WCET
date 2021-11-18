/* Author: Oussama Oulkaid */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//the following data structure represents a basic block composing a control-flow graph
typedef struct basic_block {
    int id;                             //identifier of the basic block
    int size;                           //number of instructions that form the basic block
    char addr_start[5];                 //string contrining the start address of the basic block
    struct basic_block *bb_branch;      //pointer to the basic block corresponding to branching address (may be null for some nodes)
    struct basic_block *bb_continue;    //pointer to the basic block when no branching is performed (is only null of the EXIT(v) node)
} basic_block;

basic_block *cfg;   //root of the control-flow graph
int cfg_size = 0;   //initial size if the control-flow graph
int id = -1;        //the global value of basic block identifier that will keep updating as the parsing is going on

//this function is used to get a new identifier to be assigned when a new basic block is being created
int get_new_id(void) {
    id++;
    return id;
}   

//respective branch and continue addresses for each basic block, to be indexed by the corresponding node ID in the CFG
//(the data structure is meant to not contain these elements, that's we have created them just to help us with the CFG reconstruction later on) 
char **addr_branch, **addr_continue;

//this is used whenever a new node must be created
void insert_node(char *addr_start_i, char *addr_branch_i, char *addr_continue_i, int size_i) {
    //re-allocate the required memory
    cfg = (basic_block*)realloc(cfg, (cfg_size+1)*sizeof(basic_block));

    //fill the elements : id, size, addr_start, and 
    cfg[cfg_size].id = get_new_id();
    cfg[cfg_size].size = size_i;
    strcpy(cfg[cfg_size].addr_start, addr_start_i);

    //expand the global arrays (addr_branch, addr_continue), and append the new branch & continue addresses of the node being created
    addr_branch = (char**)realloc(addr_branch, (cfg_size+1)*sizeof(char*));
    addr_continue = (char**)realloc(addr_continue, (cfg_size+1)*sizeof(char*));
    //
    addr_branch[cfg_size] = (char*)malloc(5*sizeof(char));
    addr_continue[cfg_size] = (char*)malloc(5*sizeof(char));
    //
    strcpy(addr_branch[cfg_size], addr_branch_i);
    strcpy(addr_continue[cfg_size], addr_continue_i);

    //initiate (by NULL) bb_branch, bb_continue
    cfg[cfg_size].bb_branch = NULL;
    cfg[cfg_size].bb_continue = NULL;

    //update the CFG size
    cfg_size++;
}

//this data structure contains the status flags that will be used to perform the parsing
typedef struct flags {
    bool _start;    //tells if the "ConroleurPorte_step" function is achieved by the parser
    bool bb_end;    //tells if the end of the current basic block is achieved by the parser
    bool end_step;  //tells if the end of the "ConroleurPorte_step" function is achieved by the parser
} flags;

//this data structure is used to contain all information about the current basic block
typedef struct tmp_block {
    char addr_start[5];
    char addr_branch[5];
    char addr_continue[5];
    int size;
} tmp_block;


int main() {
    FILE *file_read; //the assembly code file to be read
    char ch[200];    //a string that will contain (at a time) a line from the assembly code file

    FILE *output;       //will contain the CFG after reconstruction 
    FILE *summerize;    //will contain a reduced file representing the CFG as "csv" file ; easy to read for plotting perpuses
    char buffer[100];   //a temporary buffer ; to be used to write to the output files

    flags detected; 
    tmp_block *current_block;
    current_block = (tmp_block*)malloc(sizeof(tmp_block));

    //initialize the CFG, by inserting a virtual startup basic block
    cfg = (basic_block*)malloc(sizeof(basic_block));
    addr_branch = (char**)malloc(sizeof(char*));
    addr_continue = (char**)malloc(sizeof(char*));

    file_read = fopen("DoorControl.ass", "r");
    output = fopen("output.cfg", "w");

    if (file_read == NULL) {
        perror("Error while opening file.\n");
		exit(EXIT_FAILURE);
    }

    fgets(ch, 200 , file_read);     //get the first line of the assembly code file
    detected._start = false;
    while (!feof(file_read) && !detected._start) {
        if (strncmp("<ControleurPorte_step>:", ch+9, sizeof("<ControleurPorte_step>:")-1) == 0) {
            detected._start = true;
        }

        fgets(ch, 200, file_read);
    }

    if (detected._start) {
        //insert INIT(v) node
        *current_block->addr_start = '\0';
        *current_block->addr_branch = '\0';
        current_block->size = 0;

        snprintf(buffer, 100, "BB %d: \n", id+1);
        fputs(buffer, output);

        detected.end_step = false;
        do {
            if (strncmp(ch,"\n", 1) == 0) {
                detected.end_step = true;
                *current_block->addr_branch = '\0';     // the last bb have no branching
                *current_block->addr_continue = '\0';   // and no continue (except the EXIT(v) node)
            }
            else {
                strncpy(current_block->addr_continue, ch+4, 4);
                current_block->addr_continue[4] = '\0';
            }

            insert_node(current_block->addr_start, current_block->addr_branch, current_block->addr_continue, current_block->size);
            printf("INFO <new node>: node id: %d, addr_start: %s, addr_branch: %s, addr_continue: %s, size: %d\n", cfg[cfg_size-1].id, cfg[cfg_size-1].addr_start, addr_branch[cfg_size-1], addr_continue[cfg_size-1], cfg[cfg_size-1].size); 

            strcpy(current_block->addr_start, current_block->addr_continue);
            current_block->size = 0;
            
            snprintf(buffer, 100, "BB %d: \n", id+1); //this corresponds to the following node to be added to the data structrue
            fputs(buffer, output);

            detected.bb_end = false;

            while (!feof(file_read) && !detected.bb_end && detected._start == true && !detected.end_step) {

                current_block->size += 1;
                for (int i=10; i<30; i++) { // this interval is given by estimation, so that the operand of the instruction will exist within it
                    if (strncmp("	beq	", ch+i, 5) == 0 || strncmp("	bne	", ch+i, 5) == 0) {
                        detected.bb_end = true;
                        strncpy(current_block->addr_branch, ch+i+sizeof("beq")+1, 4);
                        current_block->addr_branch[4] = '\0';
                        i=30; //force break loop
                    }

                    else if (strncmp("	b	", ch+i, 3) == 0) {
                        detected.bb_end = true;
                        strncpy(current_block->addr_branch, ch+i+sizeof("b")+1, 4);
                        current_block->addr_branch[4] = '\0';
                        i=30; //force break loop
                    }

                    else if (strncmp("	bl	", ch+i, 4) == 0) {
                        detected.bb_end = true;
                        strncpy(current_block->addr_branch, ch+i+sizeof("bl")+1, 4);
                        current_block->addr_branch[4] = '\0';
                        i=30; //force break loop
                    }
                }

                snprintf(buffer, 100, ch);
                fputs(buffer, output);  

                fgets(ch, 200, file_read);

                if (strncmp(ch,"\n", 1) == 0) { // end of "ControleurPorte_step"
                    detected.bb_end = true;
                }

                else if(!detected.bb_end) {             //we only verify the ALREADING EXISTING @ if we haven't just detected the end of a basic (otherwise it just does useless verification)
                    for (int m=0; m<cfg_size; m++) {    //detect block that start with a branching address of another block
                        if (strncmp(ch+4, addr_branch[m], 4) == 0) { 
                            detected.bb_end = true;
                            *current_block->addr_branch = '\0';  //force a NULL branch address
                            m = cfg_size;   //break loop
                        }
                    }
                }
            }

        } while (!feof(file_read) && !detected.end_step && detected._start == true);

        fgets(ch, 200, file_read);       
    }

    //insert EXIT(v) node
    *current_block->addr_start = '\0';
    *current_block->addr_branch = '\0';
    *current_block->addr_continue = '\0';
    current_block->size = 0;
    insert_node(current_block->addr_start, current_block->addr_branch, current_block->addr_continue, current_block->size);
    printf("INFO <exit node>: node id: %d, addr_start: %s, addr_branch: %s, addr_continue: %s, size: %d\n", cfg[cfg_size-1].id, cfg[cfg_size-1].addr_start, addr_branch[cfg_size-1], addr_continue[cfg_size-1], cfg[cfg_size-1].size); 



    /* -------------------------------- */
    /*      THE CFG RECONSTRUCTION      */
    /* -------------------------------- */
    
    //link the basic block by branch addresses
    for (int i=0; i<cfg_size; i++) {
        for (int j=0; j<cfg_size; j++) {
            if (*addr_branch[i] != '\0' && strcmp(addr_branch[i], cfg[j].addr_start) == 0) {
                cfg[i].bb_branch = cfg+j;
            }
        }
    }

    //link the basic block by continue addresses
    for (int i=0; i<cfg_size-1; i++) { //ignoring the EXIT(v) node, since it doesn't have a continue address
        cfg[i].bb_continue = cfg+i+1;
    }



    /* -------------------------------- */
    /*     OUTPUT CFG AS A CSV FILE     */
    /* -------------------------------- */

    //TRAVERSAL & RECONSTRUCTION OF CFG AS A CSV FILE TO BE PROCESSED BY display_cfg.py
    //the out file has the following columns : current_bb_id | branch_bb_id | continue_bb_id
    summerize = fopen("summerize.csv", "w");

    snprintf(buffer, 100, "current_bb_id, branch_bb_id, continue_bb_id\n");
    fputs(buffer, summerize);

    //FIXME: Some branches doesn't exist within the scope of "controleurPorte_step" ; we MUST create additional nodes for them.
    //Here, we use the value "-1" to indicate non existing of an address (branch or continue) for a given basic block
    for (int i=0; i<cfg_size; i++) {
        snprintf(buffer, 100, "%d, %d, %d\n", cfg[i].id, \
                                              (cfg[i].bb_branch != NULL) ? cfg[i].bb_branch->id : -1, \
                                              (cfg[i].bb_continue != NULL) ? cfg[i].bb_continue->id : -1); //current_bb_id, branch_bb_id, continue_bb_id
        fputs(buffer, summerize);
    }
        
    
    //close opened files
    fclose(file_read);
    fclose(output);
    fclose(summerize);

    return 0;
}