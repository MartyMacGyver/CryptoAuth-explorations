/*
 * sha204_example_encrypted_write.h
 *
 * Created: 1/27/2013 4:52:54 PM
 *  Author: rocendob
 */ 


#ifndef SHA204_EXAMPLE_ENCRYPTED_WRITE_H_
#define SHA204_EXAMPLE_ENCRYPTED_WRITE_H_

#define OP_4_BYTES          (0 << 7)
#define OP_32_BYTES         (1 << 7)
#define CONFIG_ZONE         (0 << 0)
#define OTP_ZONE            (1 << 0)
#define DATA_ZONE           (2 << 0)

#define OFFSET_BYTE_0       (0 << 0)
#define OFFSET_BYTE_4       (1 << 0)
#define OFFSET_BYTE_8       (2 << 0)
#define OFFSET_BYTE_12      (3 << 0)
#define OFFSET_BYTE_16      (4 << 0)
#define OFFSET_BYTE_20      (5 << 0)
#define OFFSET_BYTE_24      (6 << 0)
#define OFFSET_BYTE_28      (7 << 0)

#define OFFSET_BLOCK_0      (0 << 3)
#define OFFSET_BLOCK_1      (1 << 3)
#define OFFSET_BLOCK_2      (2 << 3)
#define OFFSET_BLOCK_3      (3 << 3)
#define OFFSET_BLOCK_4      (4 << 3)
#define OFFSET_BLOCK_5      (5 << 3)
#define OFFSET_BLOCK_6      (6 << 3)
#define OFFSET_BLOCK_7      (7 << 3)
#define OFFSET_BLOCK_8      (8 << 3)
#define OFFSET_BLOCK_9      (9 << 3)
#define OFFSET_BLOCK_10     (10 << 3)
#define OFFSET_BLOCK_11     (11 << 3)
#define OFFSET_BLOCK_12     (12 << 3)
#define OFFSET_BLOCK_13     (13 << 3)
#define OFFSET_BLOCK_14     (14 << 3)
#define OFFSET_BLOCK_15     (15 << 3)



#endif /* SHA204_EXAMPLE_ENCRYPTED_WRITE_H_ */