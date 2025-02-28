#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_LABELS 1000


typedef struct {
    char label[50];
    int address;
} Label;

Label labels[MAX_LABELS];
int label_count = 0;


void trim_whitespace(char *str) {
    char *start = str;
    while (isspace((unsigned char)*start)) start++; 

    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--; 

    *(end + 1) = '\0'; 
    memmove(str, start, strlen(start) + 1);
}

void setX0ToZero(int* registers) {
    registers[0] = 0; 
}


void print_binary_debug(unsigned int value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
    }
    printf("\n");
}


void to_lowercase(char *str) {
    for (; *str; ++str) *str = tolower(*str);
}



int find_label_address(const char *label) {
    for (int i = 0; i < label_count; i++) {
      
        if (strcasecmp(labels[i].label, label) == 0) {
            return labels[i].address;
        }
    }
  
    return -1; 
}


void generate_output_filenames(const char *input_filename, char *output_filename, char *trace_filename) {
   
    char *dot_position = strrchr(input_filename, '.');
    
    if (dot_position != NULL && strcmp(dot_position, ".s") == 0) {
     
        size_t basename_length = dot_position - input_filename;
        strncpy(output_filename, input_filename, basename_length);
        strncpy(trace_filename, input_filename, basename_length);
        output_filename[basename_length] = '\0';
        trace_filename[basename_length] = '\0';
    } else {
       
        strcpy(output_filename, input_filename);
        strcpy(trace_filename, input_filename);
    }

    
    strcat(output_filename, ".o");
    strcat(trace_filename, ".trace");
}


unsigned int convert_ADD(int rd, int rs1, int rs2);
unsigned int convert_SUB(int rd, int rs1, int rs2);
unsigned int convert_AND(int rd, int rs1, int rs2);
unsigned int convert_OR(int rd, int rs1, int rs2);
unsigned int convert_XOR(int rd, int rs1, int rs2);
unsigned int convert_SLL(int rd, int rs1, int rs2);
unsigned int convert_SRL(int rd, int rs1, int rs2);
unsigned int convert_SRA(int rd, int rs1, int rs2);


unsigned int convert_ADDI(int rd, int rs1, int imm);
unsigned int convert_ANDI(int rd, int rs1, int imm);
unsigned int convert_ORI(int rd, int rs1, int imm);
unsigned int convert_XORI(int rd, int rs1, int imm);
unsigned int convert_SLLI(int rd, int rs1, int imm);
unsigned int convert_SRLI(int rd, int rs1, int imm);
unsigned int convert_SRAI(int rd, int rs1, int imm);
unsigned int convert_LW(int rd, int rs1, int imm);
unsigned int convert_JALR(int rd, int rs1, int imm);


unsigned int convert_SW(int rs1, int rs2, int imm);


unsigned int convert_JAL(int rd, int imm);


unsigned int convert_BGE(int rs1, int rs2, int offset);
unsigned int convert_BEQ(int rs1, int rs2, int offset);
unsigned int convert_BNE(int rs1, int rs2, int offset);
unsigned int convert_BLT(int rs1, int rs2, int offset);


void print_binary(FILE *output_file, unsigned int value);


void process_labels_and_generate_o(FILE *file, FILE *output_file, unsigned int *pc);
    
int main(void)
{
    char filename[100]; 
    FILE *file, *output_file, *trace_file;      
    int syntax_error_flag; 
    int exit_flag = 0; 
     unsigned int pc = 1000; 
      char line[256];  
  
    int registers[32] = {0};
    
      
        for (int i = 0; i < 32; i++) {
            registers[i] = 0;
        }

     registers[1] = 1;
     registers[2] = 2;
     registers[3] = 3;
     registers[4] = 4;
     registers[5] = 5;
     registers[6] = 6;
     setX0ToZero(registers); 

   while(1){
    
   
    printf("Enter Input File Name: ");
    scanf("%s", filename);

    
    if(strcmp(filename, "terminate")==0){
        printf("program terminated\n");
        break;
    }

 
    file = fopen(filename, "r");
    if(file == NULL){
        printf("Input file does not exist!!\n");
        continue;
    }

  
    char output_filename[100];
    char trace_filename[100];


    generate_output_filenames(filename, output_filename, trace_filename);

        output_file = fopen(output_filename, "w");
        trace_file = fopen(trace_filename, "w");

     if (!output_file || !trace_file) {
            printf("Could not create output or trace file.\n");
            fclose(file);
            continue;
        }

        
        process_labels_and_generate_o(file, output_file, &pc);
        


        rewind(file); 
        pc = 1000;

    syntax_error_flag = 0; 
  
   
     
    while(fgets(line, sizeof(line), file)){
        
        line[strcspn(line, "\n")] = '\0';
        trim_whitespace(line);           
        
    if (strlen(line) == 0 || strchr(line, ':') != NULL) {
       
        continue;
    }

              
    if (strlen(line) == 0) {
    
        continue; 
    }
        
     
    to_lowercase(line);
        
       
            fprintf(trace_file, "%d\n", pc);



   
   

          
            if (strchr(line, ':') != NULL) {
                continue;
            }

            
        

        unsigned int machine_code = 0;
    
    trim_whitespace(line); 
   
    if (strcasecmp(line, "EXIT") == 0) {
       

        machine_code = 0xFFFFFFFF;


       
        exit_flag = 1; 
        break;         
    }

      
        if(strncasecmp(line, "ADDI", 4) == 0){
            int rd, rs1, imm;
             if (sscanf(line, "addi x%d, x%d, %d", &rd, &rs1, &imm) != 3)
                {
                    
                    syntax_error_flag = 1; 
                    break;
                }
            machine_code = convert_ADDI(rd, rs1, imm);
            registers[rd] = registers[rs1] + imm;
        
            
        }
        else if(strncasecmp(line, "ANDI", 4) == 0){
            int rd, rs1, imm;
             if (sscanf(line, "andi x%d, x%d, %d", &rd, &rs1, &imm) != 3)
                {
                    
                    syntax_error_flag = 1; 
                    break;
                }
            machine_code = convert_ANDI(rd, rs1, imm);
            registers[rd] = registers[rs1] & imm;
            
        }
        else if(strncasecmp(line, "ORI", 3) == 0){
            int rd, rs1, imm;
             if (sscanf(line, "ori x%d, x%d, %d", &rd, &rs1, &imm) != 3)
                {
                    
                    syntax_error_flag = 1; 
                    break;
                }
            machine_code = convert_ORI(rd, rs1, imm);
              registers[rd] = registers[rs1] | imm; 
        }
        else if(strncasecmp(line, "XORI", 4) == 0){
            int rd, rs1, imm;
             if (sscanf(line, "xori x%d, x%d, %d", &rd, &rs1, &imm) != 3)
                {
                   
                    syntax_error_flag = 1; 
                    break;
                }
            machine_code = convert_XORI(rd, rs1, imm);
             registers[rd] = registers[rs1] ^ imm; 
        }
     
        else if (strncasecmp(line, "JALR", 4) == 0) {
    int rd, rs1, imm;

    
    if (sscanf(line, "jalr x%d, %d(x%d)", &rd, &imm, &rs1) == 3) {
        
        unsigned int new_pc = (registers[rs1] + imm) & ~1;

       
        

        if (rd != 0) {
            registers[rd] = pc + 4;
        }

        pc = new_pc;

        rewind(file);
        unsigned int current_pc = 1000;
        while (current_pc < pc && fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = '\0'; 
            trim_whitespace(line);
            if (strlen(line) == 0 || strchr(line, ':') != NULL) continue; 
            current_pc += 4;
        }

      
        continue;
    } else {
      
        syntax_error_flag = 1;  
        break;
    }
}



        else if(strncasecmp(line, "LW", 2) == 0){
            int rd, rs1, imm;
             if (sscanf(line, "lw x%d, %d(x%d)", &rd, &imm, &rs1) != 3)
                {
                  
                    syntax_error_flag = 1; 
                    break;
                }
            machine_code = convert_LW(rd, rs1, imm);
        }
        else if(strncasecmp(line, "SLLI", 4) == 0){ 
            int rd, rs1, imm;
             if (sscanf(line, "slli x%d, x%d, %d", &rd, &rs1, &imm) != 3)
                {
                   
                    syntax_error_flag = 1; 
                    break;
                }
            machine_code = convert_SLLI(rd, rs1, imm);
            registers[rd] = registers[rs1] << imm; 
        }
        else if(strncasecmp(line, "SRLI", 4) == 0){
            int rd, rs1, imm;
            if (sscanf(line, "srli x%d, x%d, %d", &rd, &rs1, &imm) != 3)
                {
                   
                    syntax_error_flag = 1; 
                    break;
                }
            machine_code = convert_SRLI(rd, rs1, imm);
            registers[rd] = (unsigned int)registers[rs1] >> imm; 
        }
        else if(strncasecmp(line, "SRAI", 4) == 0){
            int rd, rs1, imm;

             if (sscanf(line, "srai x%d, x%d, %d", &rd, &rs1, &imm) != 3)
                {
                    
                    syntax_error_flag = 1; 
                    break;
                }
            machine_code = convert_SRAI(rd, rs1, imm);
            registers[rd] = registers[rs1] >> imm; 
        }
         else if(strncasecmp(line, "SW", 2) == 0){
            int rs1, rs2, imm;
            
             if (sscanf(line, "sw x%d, %d(x%d)", &rs2, &imm, &rs1) != 3)
                {
                   
                    syntax_error_flag = 1; 
                    break;
                }
            machine_code = convert_SW(rs1, rs2, imm);
            
        }

       
        else if(strncasecmp(line, "ADD", 3) == 0){ 
            int rd, rs1, rs2;
             
              if (sscanf(line, "add x%d, x%d, x%d", &rd, &rs1, &rs2) != 3)
                {
                   
                    syntax_error_flag = 1; 
                    break;
                }
             machine_code = convert_ADD(rd, rs1, rs2);
             registers[rd] = registers[rs1] + registers[rs2];
          
        }
        else if(strncasecmp(line, "SUB", 3) == 0){ 
            int rd, rs1, rs2;
          
             if (  sscanf(line, "sub x%d, x%d, x%d", &rd, &rs1, &rs2) != 3)
                {
                    
                    syntax_error_flag = 1; 
                    break;
                }
             machine_code = convert_SUB(rd, rs1, rs2);
             registers[rd] = registers[rs1] - registers[rs2];
        }
        else if(strncasecmp(line, "AND", 3) == 0){ 
            int rd, rs1, rs2;
          
             if (sscanf(line, "and x%d, x%d, x%d", &rd, &rs1, &rs2)!= 3)
                {
                  
                    syntax_error_flag = 1; 
                    break;
                }
             machine_code = convert_AND(rd, rs1, rs2);
             registers[rd] = registers[rs1] & registers[rs2];
        }
        
        else if(strncasecmp(line, "OR", 2) == 0){ 
            int rd, rs1, rs2;
             if (sscanf(line, "or x%d, x%d, x%d", &rd, &rs1, &rs2)!= 3)
                {
                    
                    syntax_error_flag = 1; 
                    break;
                }
             machine_code = convert_OR(rd, rs1, rs2);
             registers[rd] = registers[rs1] | registers[rs2];
        }

        else if(strncasecmp(line, "XOR", 3) == 0){ 
            int rd, rs1, rs2;
             if (sscanf(line, "xor x%d, x%d, x%d", &rd, &rs1, &rs2)!= 3)
                {
                   
                syntax_error_flag = 1; 
                    break;
                }
             machine_code = convert_XOR(rd, rs1, rs2);
             registers[rd] = registers[rs1] ^ registers[rs2];
        }

        else if(strncasecmp(line, "SLL", 3) == 0){ 
            int rd, rs1, rs2;
             if (sscanf(line, "sll x%d, x%d, x%d", &rd, &rs1, &rs2)!= 3)
                {
                    
                    syntax_error_flag = 1; 
                    break;
                }
             machine_code = convert_SLL(rd, rs1, rs2);
             registers[rd] = registers[rs1] << (registers[rs2] & 0x1F); 
        }

        else if(strncasecmp(line, "SRL", 3) == 0){ 
            int rd, rs1, rs2;
             if (sscanf(line, "srl x%d, x%d, x%d", &rd, &rs1, &rs2)!= 3)
                {
                   
                    syntax_error_flag = 1; 
                    break;
                }
             machine_code = convert_SRL(rd, rs1, rs2);
             registers[rd] = (unsigned int)registers[rs1] >> (registers[rs2] & 0x1F); 
        }

        else if(strncasecmp(line, "SRA", 3) == 0){ 
            int rd, rs1, rs2;
            if (sscanf(line, "sra x%d, x%d, x%d", &rd, &rs1, &rs2)!= 3)
                {
                     
                    syntax_error_flag = 1; 
                    break;
                }
             machine_code = convert_SRA(rd, rs1, rs2);
             registers[rd] = registers[rs1] >> (registers[rs2] & 0x1F); 
        }

      
        else if (strncasecmp(line, "JAL", 3) == 0) {
    int rd;
    char label[50];

  
    if (sscanf(line, "jal x%d, %s", &rd, label) == 2) {
        
        int label_address = find_label_address(label);
        if (label_address == -1) {
            printf("Syntax Error! Label '%s' not found\n", label);
            syntax_error_flag = 1; 
            break;
        }

        
        int offset = label_address - pc;

       
        machine_code = convert_JAL(rd, offset);

       
        registers[rd] = pc + 4;

       

     
        rewind(file); 
        pc = 1000;    

        
        while (pc < label_address && fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = '\0'; 
            trim_whitespace(line);
            if (strlen(line) == 0 || strchr(line, ':') != NULL) continue; 
            pc += 4; 
        }

       
        continue;
    } else {
    
        syntax_error_flag = 1; 
        break;
    }
}



     
        
   else if (strncasecmp(line, "BGE", 3) == 0) {
    int rs1, rs2;
    char label[50];

    
    if (sscanf(line, "bge x%d, x%d, %s", &rs1, &rs2, label) != 3) {
       
        syntax_error_flag = 1; 
        break;
    }

    
    int label_address = find_label_address(label);
    if (label_address == -1) {

        syntax_error_flag = 1;
        break;
    }

   
    int current_pc = pc;
    int offset = label_address - current_pc;

  
    machine_code = convert_BGE(rs1, rs2, offset); 

    


if (registers[rs1] >= registers[rs2]) {
  
    fseek(file, 0, SEEK_SET); 
    pc = 1000; 


    while (pc < label_address && fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; 
        trim_whitespace(line);
        if (strlen(line) == 0 || strchr(line, ':') != NULL) continue; 
        pc += 4; 

       

    }
    continue; 
} else {
   

    pc += 4; 
    continue; 
}

}

else if (strncasecmp(line, "BEQ", 3) == 0) {
    int rs1, rs2;
    char label[50];

    
    if (sscanf(line, "beq x%d, x%d, %s", &rs1, &rs2, label) != 3) {
   
        syntax_error_flag = 1; 
        break;
    }

    int label_address = find_label_address(label);
    if (label_address == -1) {
   
        syntax_error_flag = 1;
        break;
    }

    int current_pc = pc;
    int offset = label_address - current_pc;

   
    machine_code = convert_BEQ(rs1, rs2, offset); 

    


if (registers[rs1] == registers[rs2]) {

    fseek(file, 0, SEEK_SET); 
    pc = 1000; 


    while (pc < label_address && fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; 
        trim_whitespace(line);
        if (strlen(line) == 0 || strchr(line, ':') != NULL) continue; 
        pc += 4; 

       

    }
    continue; 
} else {


    pc += 4; 
    continue; 
}

}

else if (strncasecmp(line, "BNE", 3) == 0) {
    int rs1, rs2;
    char label[50];

 
    if (sscanf(line, "bne x%d, x%d, %s", &rs1, &rs2, label) != 3) {
      
        syntax_error_flag = 1; 
        break;
    }

    
    int label_address = find_label_address(label);
    if (label_address == -1) {
     
        syntax_error_flag = 1;
        break;
    }

   
    int current_pc = pc;
    int offset = label_address - current_pc;

  
    machine_code = convert_BNE(rs1, rs2, offset); 

   


if (registers[rs1] != registers[rs2]) {
  
    fseek(file, 0, SEEK_SET);
    pc = 1000; 

    while (pc < label_address && fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; 
        trim_whitespace(line);
        if (strlen(line) == 0 || strchr(line, ':') != NULL) continue; 
        pc += 4; 

       

    }
    continue; 
} else {
    

    pc += 4; 
    continue; 
}

}

else if (strncasecmp(line, "BLT", 3) == 0) {
    int rs1, rs2;
    char label[50];

   
    if (sscanf(line, "blt x%d, x%d, %s", &rs1, &rs2, label) != 3) {
        
        syntax_error_flag = 1; 
        break;
    }

   
    int label_address = find_label_address(label);
    if (label_address == -1) {
     
        syntax_error_flag = 1;
        break;
    }

   
    int current_pc = pc;
    int offset = label_address - current_pc;

   
    machine_code = convert_BLT(rs1, rs2, offset); 

  


if (registers[rs1] < registers[rs2]) {
  
    fseek(file, 0, SEEK_SET); 
    pc = 1000; 

    while (pc < label_address && fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; 
        trim_whitespace(line);
        if (strlen(line) == 0 || strchr(line, ':') != NULL) continue; 
        pc += 4; 

       

    }
    continue;
} else {
  

    pc += 4; 
    continue; 
}

}
        else
        {
           
            syntax_error_flag = 1; 
                break;
        }

     

     
      
     
        pc += 4;
    }
       
    fclose(file);


       
        if (syntax_error_flag) {
            printf("Syntax Error!!\n");
            if (output_file) fclose(output_file);
            if (trace_file) fclose(trace_file);
            remove(output_filename);
            remove(trace_filename);
            continue; 
        }
        
    fclose(output_file);
    fclose(trace_file);

    if(exit_flag == 1){
        break;
    }
   }

    return 0;
}


unsigned int convert_ADD(int rd, int rs1, int rs2){
    unsigned int opcode = 0b0110011;
    unsigned int funct3 = 0b000;
    unsigned int funct7 = 0b0000000;
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;

}

unsigned int convert_SUB(int rd, int rs1, int rs2){
    unsigned int opcode = 0b0110011;
    unsigned int funct3 = 0b000;
    unsigned int funct7 = 0b0100000;
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int convert_AND(int rd, int rs1, int rs2){
    unsigned int opcode = 0b0110011;
    unsigned int funct3 = 0b111;
    unsigned int funct7 = 0b0000000;
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int convert_OR(int rd, int rs1, int rs2){
    unsigned int opcode = 0b0110011;
    unsigned int funct3 = 0b110;
    unsigned int funct7 = 0b0000000;
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int convert_XOR(int rd, int rs1, int rs2){
    unsigned int opcode = 0b0110011;
    unsigned int funct3 = 0b100;
    unsigned int funct7 = 0b0000000;
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int convert_SLL(int rd, int rs1, int rs2){
    unsigned int opcode = 0b0110011;
    unsigned int funct3 = 0b001;
    unsigned int funct7 = 0b0000000;
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int convert_SRL(int rd, int rs1, int rs2){
    unsigned int opcode = 0b0110011;
    unsigned int funct3 = 0b101;
    unsigned int funct7 = 0b0000000;
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int convert_SRA(int rd, int rs1, int rs2){
    unsigned int opcode = 0b0110011;
    unsigned int funct3 = 0b101;
    unsigned int funct7 = 0b0100000;
    return (funct7 << 25) | (rs2 << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}


unsigned int convert_ADDI(int rd, int rs1, int imm){
    unsigned int opcode = 0b0010011;
    unsigned int funct3 = 0b000;
    unsigned int imm_12bit = imm & 0xFFF;
    if (imm < 0) { 
        imm_12bit |= 0xFFFFF000; 
    }
    return (imm_12bit << 20) | (rs1 << 15) | (funct3 << 12) | 
    (rd << 7) | opcode;
}



unsigned int convert_ANDI(int rd, int rs1, int imm){
    unsigned int opcode = 0b0010011;
    unsigned int funct3 = 0b111;
    unsigned int imm_12bit = imm & 0xFFF;
    if (imm < 0) { 
        imm_12bit |= 0xFFFFF000; 
    }
    return (imm_12bit << 20) | (rs1 << 15) | (funct3 << 12) | 
    (rd << 7) | opcode;
}




unsigned int convert_ORI(int rd, int rs1, int imm){
    unsigned int opcode = 0b0010011;
    unsigned int funct3 = 0b110;
    unsigned int imm_12bit = imm & 0xFFF;
    if (imm < 0) { 
        imm_12bit |= 0xFFFFF000; 
    }
    return (imm_12bit << 20) | (rs1 << 15) | (funct3 << 12) | 
    (rd << 7) | opcode;
}

unsigned int convert_XORI(int rd, int rs1, int imm){
    unsigned int opcode = 0b0010011;
    unsigned int funct3 = 0b100;
    unsigned int imm_12bit = imm & 0xFFF;
    return (imm_12bit << 20) | (rs1 << 15) | (funct3 << 12) | 
    (rd << 7) | opcode;
}

unsigned int convert_SLLI(int rd, int rs1, int imm){ 
    unsigned int opcode = 0b0010011;
    unsigned int funct3 = 0b001;
    unsigned int funct7 = 0b0000000;
    unsigned int imm_5bit = imm & 0x1F; 
    return (funct7 << 25) | (imm_5bit << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int convert_SRLI(int rd, int rs1, int imm){ 
    unsigned int opcode = 0b0010011;
    unsigned int funct3 = 0b101;
    unsigned int funct7 = 0b0000000;
    unsigned int imm_5bit = imm & 0x1F; 
    return (funct7 << 25) | (imm_5bit << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int convert_SRAI(int rd, int rs1, int imm){ 
    unsigned int opcode = 0b0010011;
    unsigned int funct3 = 0b101;
    unsigned int funct7 = 0b0100000;
    unsigned int imm_5bit = imm & 0x1F; 
    return (funct7 << 25) | (imm_5bit << 20) | (rs1 << 15) | 
    (funct3 << 12) | (rd << 7) | opcode;
}

unsigned int convert_LW(int rd, int rs1, int imm){
    unsigned int opcode = 0b0000011;
    unsigned int funct3 = 0b010;
    unsigned int imm_12bit = imm & 0xFFF;
    return (imm_12bit << 20) | (rs1 << 15) | (funct3 << 12) | 
    (rd << 7) | opcode;
}

unsigned int convert_JALR(int rd, int rs1, int imm) {
    unsigned int opcode = 0b1100111; 
    unsigned int funct3 = 0b000;    
    unsigned int imm_12bit = imm & 0xFFF; 

   
    if (imm < 0) {
        imm_12bit |= 0xFFFFF000; 
    }

   
    return (imm_12bit << 20) | (rs1 << 15) | (funct3 << 12) |
           (rd << 7) | opcode;
}



unsigned int convert_SW(int rs1, int rs2, int imm){
    unsigned int opcode = 0b0100011;
    unsigned int funct3 = 0b010;
    unsigned int imm_7bit = imm & 0xFFF;
    unsigned int imm_high = (imm >> 5) & 0x7F; 
    unsigned int imm_low = imm & 0x1F;         
    return (imm_high << 25) | (rs2 << 20) | (rs1 << 15)| (funct3 << 12) | 
    (imm_low << 7) | opcode;
}


unsigned int convert_JAL(int rd, int imm) {
    unsigned int opcode = 0b1101111; 
    unsigned int imm_20bit = imm & 0xFFFFF; 

    
    if (imm < 0) {
        imm_20bit |= 0xFFF00000; 
    }

    unsigned int imm_20 = (imm_20bit >> 20) & 0x1;     
    unsigned int imm_10_1 = (imm_20bit >> 1) & 0x3FF;  
    unsigned int imm_11 = (imm_20bit >> 11) & 0x1;     
    unsigned int imm_19_12 = (imm_20bit >> 12) & 0xFF; 

    
    unsigned int machine_code = 0;
    machine_code |= (imm_20 << 31);      
    machine_code |= (imm_19_12 << 12);   
    machine_code |= (imm_11 << 20);      
    machine_code |= (imm_10_1 << 21);   
    machine_code |= (rd << 7);          
    machine_code |= opcode;             

    return machine_code;
}



unsigned int convert_BGE(int rs1, int rs2, int offset) {
    unsigned int opcode = 0b1100011;
    unsigned int funct3 = 0b101;
    unsigned int imm_11 = (offset >> 11) & 0x1;
    unsigned int imm_4_1 = (offset >> 1) & 0xF;
    unsigned int imm_10_5 = (offset >> 5) & 0x3F;
    unsigned int imm_12 = (offset >> 12) & 0x1;

    return (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) |
           (rs1 << 15) | (funct3 << 12) | (imm_4_1 << 8) |
           (imm_11 << 7) | opcode;
}

unsigned int convert_BEQ(int rs1, int rs2, int offset) {
    unsigned int opcode = 0b1100011;
    unsigned int funct3 = 0b000;
    unsigned int imm_11 = (offset >> 11) & 0x1;
    unsigned int imm_4_1 = (offset >> 1) & 0xF;
    unsigned int imm_10_5 = (offset >> 5) & 0x3F;
    unsigned int imm_12 = (offset >> 12) & 0x1;

    return (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) |
           (rs1 << 15) | (funct3 << 12) | (imm_4_1 << 8) |
           (imm_11 << 7) | opcode;
}

unsigned int convert_BNE(int rs1, int rs2, int offset) {
    unsigned int opcode = 0b1100011;
    unsigned int funct3 = 0b001;
    unsigned int imm_11 = (offset >> 11) & 0x1;
    unsigned int imm_4_1 = (offset >> 1) & 0xF;
    unsigned int imm_10_5 = (offset >> 5) & 0x3F;
    unsigned int imm_12 = (offset >> 12) & 0x1;

    return (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) |
           (rs1 << 15) | (funct3 << 12) | (imm_4_1 << 8) |
           (imm_11 << 7) | opcode;
}

unsigned int convert_BLT(int rs1, int rs2, int offset) {
    unsigned int opcode = 0b1100011;
    unsigned int funct3 = 0b100;
    unsigned int imm_11 = (offset >> 11) & 0x1;
    unsigned int imm_4_1 = (offset >> 1) & 0xF;
    unsigned int imm_10_5 = (offset >> 5) & 0x3F;
    unsigned int imm_12 = (offset >> 12) & 0x1;

    return (imm_12 << 31) | (imm_10_5 << 25) | (rs2 << 20) |
           (rs1 << 15) | (funct3 << 12) | (imm_4_1 << 8) |
           (imm_11 << 7) | opcode;
}



void print_binary(FILE *output_file, unsigned int value){
    for(int i = 31; i>=0; i--){
        fprintf(output_file, "%d", (value >> i) & 1);
    }
    fprintf(output_file, "\n");
}





void process_labels_and_generate_o(FILE *file, FILE *output_file, unsigned int *pc) {
    char line[256];
    *pc = 1000; 
    label_count = 0; 

    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; 
        trim_whitespace(line);

        if (strlen(line) == 0) continue; 

        if (strchr(line, ':') != NULL) {
           
            char label_name[50];
            sscanf(line, "%[^:]:", label_name);
            trim_whitespace(label_name);

           
            strcpy(labels[label_count].label, label_name);
            labels[label_count].address = *pc;
          
            label_count++;
        } else {
            *pc += 4; 
        }
    }

  
    rewind(file);

    *pc = 1000; 

   
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; 
        trim_whitespace(line);

        if (strlen(line) == 0 || strchr(line, ':') != NULL) continue; 

        to_lowercase(line);

        unsigned int machine_code = 0;


            if (strncasecmp(line, "ADDI", 4) == 0) {
                int rd, rs1, imm;
                if (sscanf(line, "addi x%d, x%d, %d", &rd, &rs1, &imm) == 3) {
                    machine_code = convert_ADDI(rd, rs1, imm);
                
                }
            } else if (strncasecmp(line, "ANDI", 4) == 0) {
                int rd, rs1, imm;
                if (sscanf(line, "andi x%d, x%d, %d", &rd, &rs1, &imm) == 3) {
                    machine_code = convert_ANDI(rd, rs1, imm);
                
                }
            } else if (strncasecmp(line, "ORI", 3) == 0) {
                int rd, rs1, imm;
                if (sscanf(line, "ori x%d, x%d, %d", &rd, &rs1, &imm) == 3) {
                    machine_code = convert_ORI(rd, rs1, imm);
                   
                }
            } else if (strncasecmp(line, "XORI", 4) == 0) {
                int rd, rs1, imm;
                if (sscanf(line, "xori x%d, x%d, %d", &rd, &rs1, &imm) == 3) {
                    machine_code = convert_XORI(rd, rs1, imm);
                 
                }
            } else if (strncasecmp(line, "SLLI", 4) == 0) {
                int rd, rs1, imm;
                if (sscanf(line, "slli x%d, x%d, %d", &rd, &rs1, &imm) == 3) {
                    machine_code = convert_SLLI(rd, rs1, imm);
                
                }
            } else if (strncasecmp(line, "SRLI", 4) == 0) {
                int rd, rs1, imm;
                if (sscanf(line, "srli x%d, x%d, %d", &rd, &rs1, &imm) == 3) {
                    machine_code = convert_SRLI(rd, rs1, imm);
                
                }
            } else if (strncasecmp(line, "SRAI", 4) == 0) {
                int rd, rs1, imm;
                if (sscanf(line, "srai x%d, x%d, %d", &rd, &rs1, &imm) == 3) {
                    machine_code = convert_SRAI(rd, rs1, imm);
                
                }
            } else if (strncasecmp(line, "LW", 2) == 0) {
                int rd, rs1, imm;
                if (sscanf(line, "lw x%d, %d(x%d)", &rd, &imm, &rs1) == 3) {
                    machine_code = convert_LW(rd, rs1, imm);
                   
                }
            } else if (strncasecmp(line, "JALR", 4) == 0) {
                    int rd, rs1, imm;

                  
                    if (sscanf(line, "jalr x%d, %d(x%d)", &rd, &imm, &rs1) == 3) {
                        
                        machine_code = convert_JALR(rd, rs1, imm);

                       
                      
                    }
}


          
            else if(strncasecmp(line, "SW", 2) == 0){
            int rs1, rs2, imm;
             if (sscanf(line, "sw x%d, %d(x%d)", &rs2, &imm, &rs1) == 3)
                {
                    machine_code = convert_SW(rs1, rs2, imm);
               
            
                }
            
        }

           
            else if (strncasecmp(line, "ADD", 3) == 0) {
                int rd, rs1, rs2;
                if (sscanf(line, "add x%d, x%d, x%d", &rd, &rs1, &rs2) == 3) {
                    machine_code = convert_ADD(rd, rs1, rs2);
                
                }
            } else if (strncasecmp(line, "SUB", 3) == 0) {
                int rd, rs1, rs2;
                if (sscanf(line, "sub x%d, x%d, x%d", &rd, &rs1, &rs2) == 3) {
                    machine_code = convert_SUB(rd, rs1, rs2);
               
                }
            } else if (strncasecmp(line, "AND", 3) == 0) {
                int rd, rs1, rs2;
                if (sscanf(line, "and x%d, x%d, x%d", &rd, &rs1, &rs2) == 3) {
                    machine_code = convert_AND(rd, rs1, rs2);
                
                }
            } else if (strncasecmp(line, "OR", 2) == 0) {
                int rd, rs1, rs2;
                if (sscanf(line, "or x%d, x%d, x%d", &rd, &rs1, &rs2) == 3) {
                    machine_code = convert_OR(rd, rs1, rs2);
                
                }
            } else if (strncasecmp(line, "XOR", 3) == 0) {
                int rd, rs1, rs2;
                if (sscanf(line, "xor x%d, x%d, x%d", &rd, &rs1, &rs2) == 3) {
                    machine_code = convert_XOR(rd, rs1, rs2);
               
                }
            } else if (strncasecmp(line, "SLL", 3) == 0) {
                int rd, rs1, rs2;
                if (sscanf(line, "sll x%d, x%d, x%d", &rd, &rs1, &rs2) == 3) {
                    machine_code = convert_SLL(rd, rs1, rs2);
                
                }
            } else if (strncasecmp(line, "SRL", 3) == 0) {
                int rd, rs1, rs2;
                if (sscanf(line, "srl x%d, x%d, x%d", &rd, &rs1, &rs2) == 3) {
                    machine_code = convert_SRL(rd, rs1, rs2);
                 
                }
            } else if (strncasecmp(line, "SRA", 3) == 0) {
                int rd, rs1, rs2;
                if (sscanf(line, "sra x%d, x%d, x%d", &rd, &rs1, &rs2) == 3) {
                    machine_code = convert_SRA(rd, rs1, rs2);
                
                }
            }

          
            else if (strncasecmp(line, "BGE", 3) == 0) {
                int rs1, rs2;
                char label[50];
                if (sscanf(line, "bge x%d, x%d, %s", &rs1, &rs2, label) == 3) {
                    int label_address = find_label_address(label);
                    if (label_address != -1) {
                        int offset = (label_address - *pc);  
                        machine_code = convert_BGE(rs1, rs2, offset);
                     
                    }
                }
            }

            else if (strncasecmp(line, "BEQ", 3) == 0) {
                int rs1, rs2;
                char label[50];
                if (sscanf(line, "beq x%d, x%d, %s", &rs1, &rs2, label) == 3) {
                    int label_address = find_label_address(label);
                    if (label_address != -1) {
                        int offset = (label_address - *pc);   
                        machine_code = convert_BEQ(rs1, rs2, offset);
                     
                    }
                }
            }

            else if (strncasecmp(line, "BNE", 3) == 0) {
                int rs1, rs2;
                char label[50];
                if (sscanf(line, "bne x%d, x%d, %s", &rs1, &rs2, label) == 3) {
                    int label_address = find_label_address(label);
                    if (label_address != -1) {
                        int offset = (label_address - *pc);   
                        machine_code = convert_BNE(rs1, rs2, offset);
                    
                    }
                }
            }

             else if (strncasecmp(line, "BLT", 3) == 0) {
                int rs1, rs2;
                char label[50];
                if (sscanf(line, "blt x%d, x%d, %s", &rs1, &rs2, label) == 3) {
                    int label_address = find_label_address(label);
                    if (label_address != -1) {
                        int offset = (label_address - *pc);  
                        machine_code = convert_BLT(rs1, rs2, offset);
                   
                    }
                }
            }

           
            else if (strncasecmp(line, "JAL", 3) == 0) {
    int rd;
    char label[50];

  
    if (sscanf(line, "jal x%d, %s", &rd, label) == 2) {
        int label_address = find_label_address(label);
        if (label_address != -1) {
            int imm = label_address - *pc; 
            machine_code = convert_JAL(rd, imm);

         
          
        } else {
         
        }
    }
}


            else if (strcasecmp(line, "EXIT") == 0) {
                machine_code = 0xFFFFFFFF;
            }

           
            if (machine_code != 0) {
                print_binary(output_file, machine_code);
            }

    
            *pc += 4;
        
    }
}

