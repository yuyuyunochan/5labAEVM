// #include "mySimpleComputer.h"
// #include <stdio.h>
// #include <stdlib.h>

// // Макросы для номеров регистров флагов
// #define FLAG_ZERO   (1 << 0)   // Флаг "равно нулю"
// #define FLAG_SIGN   (1 << 1)   // Флаг "знака" (положительное/отрицательное
// число) #define FLAG_CARRY  (1 << 2)   // Флаг переноса

// // Функция для вывода содержимого ячейки памяти
// void printCell(int address) {
//     // Проверка корректности адреса (пример: допустимые адреса от 0 до 127)
//     if (address < 0 || address > 128) {
//         printf("ERROR\tНеверный адрес ячейки\n");
//         return;
//     }

//     int sign = 0, command = 0, operand = 0;
//     sc_commandDecode(memory[address], &sign, &command, &operand);

//     // Вывод декодированного значения
//     printf("+%.2X%.2X ", command, operand);
// }

// // Функция для вывода значений флагов
// void printFlags(void) {
//     int value;

//     sc_regGet(OVERFLOW, &value);
//     char flag_over = (value) ? 'P' : '_';

//     sc_regGet(DIVISION_ERR_BY_ZERO, &value);
//     char flag_zero = (value) ? '0' : '_';

//     sc_regGet(OUT_OF_MEMORY, &value);
//     char flag_out = (value) ? 'M' : '_';

//     sc_regGet(IGNORING_TACT_PULSES, &value);
//     char flag_ign = (value) ? 'T' : '_';

//     sc_regGet(INCORRECT_COMMAND, &value);
//     char flag_inc = (value) ? 'E' : '_';

//     printf("Flags: %c %c %c %c %c\n", flag_over, flag_zero, flag_out,
//     flag_ign, flag_inc);
// }

// // Функция для вывода декодированной команды
// void printDecodedCommand(int value) {
//     printf("dec: %.5d | oct: %o | hex: %X\tbin: ", value, value, value);
//     for (int i = 14; i >= 0; i--) {
//         printf("%d", (value >> i) & 1);
//     }
//     printf("\n");
// }

// // Функция для вывода значения аккумулятора
// void printAccumulator(void) {
//     printf("Accumulator: %d\n", accumulator);
// }

// // Функция для вывода значений счетчиков
// void printCounters(void) {
//     printf("Instruction Counter: %d\n", icounter);
// }
