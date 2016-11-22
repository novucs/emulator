/**
 * @file main.c
 * @author William Randall (16002374)
 * @date 11th November 2016
 * @brief Complete 60 marks emulator for CaNS assignment 1 at UWE.
 */

#include <winsock2.h>
#include <stdio.h>
#include "main.h"

SOCKADDR_IN server_addr;
SOCKADDR_IN client_addr;
SOCKET sock;
WSADATA data;

char input_buffer[MAX_BUFFER_SIZE];
char hex_file[MAX_BUFFER_SIZE];
char trc_file[MAX_BUFFER_SIZE];

BYTE registers[5];
BYTE index_registers[2];
BYTE flags;
WORD program_counter;
WORD stack_pointer;

BYTE memory[MEMORY_SIZE];

bool halt = false;

void (*opcodes[])() = {
  nop, decx, incx, decy, incy, clc, stc, cli,
  sti, nop, ldaa_imm, ldab_imm, lx, lx, ldx_imm, ldy_imm,
  jmp, jcc, jcs, jne, jeq, jmi, jpl, jhi,
  jle, nop, ldaa_abs, ldab_abs, mvi_l, mvi_h, ldx_abs, ldy_abs,
  lods_imm, jsr, ccc, ccs, cne, ceq, cmi, cpl,
  chi, cle, ldaa_abs_x, ldab_abs_x, nop, hlt, ldx_abs_x, ldy_abs_x,
  lods_abs, adc_al, sbc_al, add_al, sub_al, cmp_al, or_al, and_al,
  xor_al, bit_al, ldaa_abs_y, ldab_abs_y, nop, nop, ldx_abs_y, ldy_abs_y,
  lods_abs_x, adc_ah, sbc_ah, add_ah, sub_ah, cmp_ah, or_ah, and_ah,
  xor_ah, bit_ah, ldaa_indir, ldab_indir, ret, nop, ldx_indir, ldy_indir,
  lods_abs_y, adc_am, sbc_am, add_am, sub_am, cmp_am, or_am, and_am,
  xor_am, bit_am, ldaa_indir_x, ldab_indir_x, swi, rti, ldx_indir_x, ldy_indir_x,
  lods_indir, adc_bl, sbc_bl, add_bl, sub_bl, cmp_bl, or_bl, and_bl,
  xor_bl, bit_bl, stos_abs, nop, move_ba, move_la, move_ha, move_ma,
  lods_indir_x, adc_bh, sbc_bh, add_bh, sub_bh, cmp_bh, or_bh, and_bh,
  xor_bh, bit_bh, stos_abs_x, move_ab, nop, move_lb, move_hb, move_mb,
  nop, adc_bm, sbc_bm, add_bm, sub_bm, cmp_bm, or_bm, and_bm,
  xor_bm, bit_bm, stos_abs_y, move_al, move_bl, nop, move_hl, move_ml,
  nop, nop, nop, sbia, sbib, cpia, cpib, oria,
  orib, nop, stos_indir, move_ah, move_bh, move_lh, nop, move_mh,
  inc_abs, dec_abs, rrc_abs, rlc_abs, sal_abs, sar_abs, lsr_abs, com_abs,
  rol_abs, rr_abs, stos_indir_x, move_am, move_bm, move_lm, move_hm, nop,
  inc_abs_x, dec_abs_x, rrc_abs_x, rlc_abs_x, sal_abs_x, sar_abs_x, lsr_abs_x, com_abs_x,
  rol_abs_x, rr_abs_x, stora_abs, storb_abs, stox_abs, stoy_abs, push_a, pop_a,
  inc_abs_y, dec_abs_y, rrc_abs_y, rlc_abs_y, sal_abs_y, sar_abs_y, lsr_abs_y, com_abs_y,
  rol_abs_y, rr_abs_y, stora_abs_x, storb_abs_x, stox_abs_x, stoy_abs_x, push_b, pop_b,
  inca, deca, rrca, rlca, sala, sara, lsra, coma,
  rola, rra, stora_abs_y, storb_abs_y, stox_abs_y, stoy_abs_y, push_f, pop_f,
  incb, decb, rrcb, rlcb, salb, sarb, lsrb, comb,
  rolb, rrb, stora_indir, storb_indir, stox_indir, stoy_indir, push_l, pop_l,
  cay, mya, csa, aba, sba, aab, sab, adcp,
  sbcp, xchg, stora_indir_x, storb_indir_x, stox_indir_x, stoy_indir_x, push_h, pop_h
};

/**
 * Checks if a flag is set.
 *
 * @param flag the flag to check.
 */
bool is_flag_set(BYTE flag) {
  return flags & flag;
}

/**
 * Updates a flag to a new state.
 *
 * @param state the new flag state.
 * @param flag the flag to update.
 */
void set_flag(bool state, BYTE flag) {
  if (state)
    flags |= flag;
  else
    flags &= ~flag;
}

/**
 * Updates the zero flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_z(BYTE data) {
  set_flag(data == 0, FLAG_Z);
}

/**
 * Updates the negative flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_n(BYTE data) {
  set_flag(data & 0x80, FLAG_N);
}

/**
 * Updates the carry flag depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flag_c(WORD data) {
  set_flag(data > 0xFF, FLAG_C);
}

/**
 * Updates the Z and N flags depending on the contents of a register.
 *
 * @param data the register value.
 */
 void set_flags_zn(BYTE data) {
   set_flag_z(data);
   set_flag_n(data);
 }

/**
 * Updates the Z, N and C flags depending on the contents of a register.
 *
 * @param data the register value.
 */
void set_flags_znc(WORD data) {
  set_flags_zn(data);
  set_flag_c(data);
}

/**
 * Gets the next instruction from memory.
 */
BYTE fetch() {
  return memory[program_counter++];
}

/**
 * Forms a memory address by joining two bytes.
 *
 * @param higher the higher byte.
 * @param lower  the lower byte.
 */
WORD join_address(BYTE higher, BYTE lower) {
  return (higher << 8) + lower;
}

/**
 * Fetch an address using absolute addressing.
 */
WORD fetch_address_abs() {
  BYTE higher = fetch();
  BYTE lower = fetch();
  return join_address(higher, lower);
}

/**
 * Fetch an address using indexed absolute addressing (X).
 */
WORD fetch_address_abs_x() {
  return fetch_address_abs() + index_registers[REGISTER_X];
}

/**
 * Fetch an address using indexed absolute addressing (Y).
 */
WORD fetch_address_abs_y() {
  return fetch_address_abs() + index_registers[REGISTER_Y];
}

/**
 * Fetch an address using indirect addressing.
 */
WORD fetch_address_indir() {
  WORD address = fetch_address_abs();
  return join_address(memory[address], memory[address + 1]);
}

/**
 * Fetch an address using indexed indirect addressing (X).
 */
WORD fetch_address_indir_x() {
  return fetch_address_indir() + index_registers[REGISTER_X];
}

/**
 * Emulates the program loaded into memory.
 */
void emulate() {
  // Initialise the program state.
  halt = false;

  // Execute each opcode recieved until program executes halt.
  while (!halt) {
    opcodes[fetch()]();
  }
}

void initialise_filenames() {
  for (int i = 0; i < MAX_FILENAME_SIZE; i++) {
    hex_file[i] = '\0';
    trc_file[i] = '\0';
  }
}

int find_dot_position(char *filename) {
  int dot_position = 0;
  int i = 0;
  char chr = filename[i];

  while (chr != '\0') {
    if (chr == '.')
      dot_position = i;

    i++;
    chr = filename[i];
  }

  return dot_position;
}

int find_end_position(char *filename) {
  int end_position = 0;
  int i = 0;
  int chr = filename[i];

  while (chr != '\0') {
    end_position = i;
    i++;
    chr = filename[i];
  }

  return end_position;
}

bool file_exists(char *filename) {
  FILE *ifp;
  bool exists = false;

  if ((ifp = fopen(filename, "r")) != NULL) {
    exists = true;
    fclose(ifp);
  }

  return exists;
}

void create_file(char *filename) {
  FILE *ofp;

  if ((ofp = fopen(filename, "w")) != NULL)
    fclose(ofp);
}

bool get_line(FILE *fp, char *buffer) {
  bool rc = false;
  bool collect = true;
  char c;
  int i = 0;

  while (collect) {
    c = getc(fp);

    switch (c) {
      case EOF:
        if (i > 0)
          rc = true;
        collect = false;
        break;

      case '\n':
        if (i > 0) {
          rc = true;
          collect = false;
          buffer[i] = '\0';
        }
        break;

      default:
        buffer[i] = c;
        i++;
        break;
    }
  }

  return rc;
}

void load_and_run(int args, char **argv) {
  char chr;
  int ln;
  int dot_position;
  int end_position;
  long i;
  FILE *ifp;
  long address;
  long load_at;
  int code;

  // Prompt for the .hex file

  printf("\n");
  printf("Enter the hex filename (.hex): ");

  if (args == 2) {
    ln = 0;
    chr = argv[1][ln];

    while (chr != '\0') {
      if (ln < MAX_FILENAME_SIZE) {
        hex_file[ln] = chr;
        trc_file[ln] = chr;
        ln++;
      }

      chr = argv[1][ln];
    }
  } else {
    ln = 0;
    chr = '\0';

    while (chr != '\n') {
      chr = getchar();

      if (chr == '\n' || ln < MAX_FILENAME_SIZE)
        continue;

      hex_file[ln] = chr;
      trc_file[ln] = chr;
      ln++;
    }
  }

  // Tidy up the file names

  dot_position = find_dot_position(hex_file);

  if (dot_position == 0) {
    end_position = find_end_position(hex_file);

    hex_file[end_position + 1] = '.';
    hex_file[end_position + 2] = 'h';
    hex_file[end_position + 3] = 'e';
    hex_file[end_position + 4] = 'x';
    hex_file[end_position + 5] = '\0';
  } else {
    hex_file[dot_position + 0] = '.';
    hex_file[dot_position + 1] = 'h';
    hex_file[dot_position + 2] = 'e';
    hex_file[dot_position + 3] = 'x';
    hex_file[dot_position + 4] = '\0';
  }

  dot_position = find_dot_position(trc_file);

  if (dot_position == 0) {
    end_position = find_end_position(trc_file);

    trc_file[end_position + 1] = '.';
    trc_file[end_position + 2] = 't';
    trc_file[end_position + 3] = 'r';
    trc_file[end_position + 4] = 'c';
    trc_file[end_position + 5] = '\0';
  } else {
    trc_file[dot_position + 0] = '.';
    trc_file[dot_position + 1] = 't';
    trc_file[dot_position + 2] = 'r';
    trc_file[dot_position + 3] = 'c';
    trc_file[dot_position + 4] = '\0';
  }

  if (!file_exists(hex_file)) {
    printf("\n");
    printf("ERROR> Input file %s does not exist!\n", hex_file);
    printf("\n");
  }

  // Clear registers and Memory
  registers[REGISTER_A] = 0;
  registers[REGISTER_B] = 0;
  registers[REGISTER_L] = 0;
  registers[REGISTER_H] = 0;
  index_registers[REGISTER_X] = 0;
  index_registers[REGISTER_Y] = 0;
  flags = 0;
  program_counter = 0;
  stack_pointer = 0;

  for (i = 0; i < MEMORY_SIZE; i++) {
    memory[i] = 0x00;
  }

  // Load hex file

  if ((ifp = fopen( hex_file, "r" )) != NULL) {
    printf("Loading file...\n\n");

    load_at = 0;

    while (get_line(ifp, input_buffer)) {
      if (sscanf(input_buffer, "L=%x", &address) == 1) {
        load_at = address;
      } else if (sscanf(input_buffer, "%x", &code) == 1) {
        if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
          memory[load_at] = (BYTE)code;
        }

        load_at++;
      } else {
        printf("ERROR> Failed to load instruction: %s \n", input_buffer);
      }
    }

    fclose(ifp);
  }

  // Emulate
  emulate();
}

void building(int args, char **argv){
  char buffer[1024];
  load_and_run(args, argv);
  sprintf(buffer, "0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X",
    memory[TEST_ADDRESS_1],
    memory[TEST_ADDRESS_2],
    memory[TEST_ADDRESS_3],
    memory[TEST_ADDRESS_4],
    memory[TEST_ADDRESS_5],
    memory[TEST_ADDRESS_6],
    memory[TEST_ADDRESS_7],
    memory[TEST_ADDRESS_8],
    memory[TEST_ADDRESS_9],
    memory[TEST_ADDRESS_10],
    memory[TEST_ADDRESS_11],
    memory[TEST_ADDRESS_12]
  );
  sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
}

void test_and_mark() {
  char buffer[1024];
  bool testing_complete;
  int len = sizeof(SOCKADDR);
  char chr;
  int i;
  int j;
  bool end_of_program;
  long address;
  long load_at;
  int code;
  int mark;
  int passed;

  printf("\n");
  printf("Automatic Testing and Marking\n");
  printf("\n");

  testing_complete = false;

  sprintf(buffer, "Test Student %s", STUDENT_NUMBER);
  sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));

  while (!testing_complete) {
    memset(buffer, '\0', sizeof(buffer));

    if (recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (SOCKADDR *)&client_addr, &len) == SOCKET_ERROR) {
      continue;
    }
    // printf("Incoming Data: %s \n", buffer);
    // printf("%d - Incoming Data, %s \n", test++, buffer);

    //if (strcmp(buffer, "Testing complete") == 1)
    if (sscanf(buffer, "Testing complete %d", &mark) == 1) {
      testing_complete = true;
      printf("Current mark = %d\n", mark);
      continue;
    } else if (sscanf(buffer, "Tests passed %d", &passed) == 1) {
      //testing_complete = true;
      printf("Passed = %d\n", passed);
      continue;
    } else if (strcmp(buffer, "Error") == 0) {
      printf("ERROR> Testing abnormally terminated\n");
      testing_complete = true;
      continue;
    }

    // Clear registers and Memory
    registers[REGISTER_A] = 0;
    registers[REGISTER_B] = 0;
    registers[REGISTER_L] = 0;
    registers[REGISTER_H] = 0;
    index_registers[REGISTER_X] = 0;
    index_registers[REGISTER_Y] = 0;
    flags = 0;
    program_counter = 0;
    stack_pointer = 0;

    for (i = 0; i < MEMORY_SIZE; i++) {
      memory[i] = 0;
    }

    // Load hex file

    i = 0;
    j = 0;
    load_at = 0;
    end_of_program = false;
    FILE *ofp;
    fopen_s(&ofp, "branch.txt", "a");

    while (!end_of_program) {
      chr = buffer[i];

      switch (chr) {
        case '\0':
          end_of_program = true;

        case ',':

          if (sscanf(input_buffer, "L=%x", &address) == 1) {
            load_at = address;
          } else if (sscanf(input_buffer, "%x", &code) == 1) {
            if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
              memory[load_at] = (BYTE)code;
              fprintf(ofp, "%02X\n", (BYTE)code);
            }

            load_at++;
          } else {
            printf("ERROR> Failed to load instruction: %s \n", input_buffer);
          }

          j = 0;
          break;

        default:
          input_buffer[j] = chr;
          j++;
          break;
      }

      i++;
    }

    fclose(ofp);

    if (load_at <= 1)
      continue;

    // Emulate
    emulate();

    // Send and store results
    sprintf(buffer, "%02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X",
      memory[TEST_ADDRESS_1],
      memory[TEST_ADDRESS_2],
      memory[TEST_ADDRESS_3],
      memory[TEST_ADDRESS_4],
      memory[TEST_ADDRESS_5],
      memory[TEST_ADDRESS_6],
      memory[TEST_ADDRESS_7],
      memory[TEST_ADDRESS_8],
      memory[TEST_ADDRESS_9],
      memory[TEST_ADDRESS_10],
      memory[TEST_ADDRESS_11],
      memory[TEST_ADDRESS_12]
    );

    sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
  }
}

int main(int argc, char *argv[]) {
  printf("\n");
  printf("Microprocessor Emulator\n");
  printf("UWE Computer and Network Systems Assignment 1\n");
  printf("\n");

  initialise_filenames();

  if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
    return 0;

  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (!sock) {
    printf("Socket creation failed!\n");
    return 0;
  }

  memset(&server_addr, 0, sizeof(SOCKADDR_IN));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
  server_addr.sin_port = htons(PORT_SERVER);

  memset(&client_addr, 0, sizeof(SOCKADDR_IN));
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  client_addr.sin_port = htons(PORT_CLIENT);

  if (argc == 2) {
    building(argc, argv);
    closesocket(sock);
    WSACleanup();
    return 0;
  }

  char input = '\0';

  while (input != 'e' && input != 'E') {
    printf("\n");
    printf("Please select option\n");
    printf("L - Load and run a hex file\n");
    printf("T - Have the server test and mark your emulator\n");
    printf("E - Exit\n");
    printf("Enter option: ");

    input = getchar();

    if (input == 0x0A)
      input = getchar();

    switch (input) {
      case 'L':
      case 'l':
        load_and_run(argc, argv);
        break;

      case 'T':
      case 't':
        test_and_mark();
        break;
    }
  }

  closesocket(sock);
  WSACleanup();
  return 0;
}
