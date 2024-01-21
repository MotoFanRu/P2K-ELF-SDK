#ifndef GAIN_H
#define GAIN_H

#define am_hw_gain_tbl_size		0x46
#define am_hw_vol_tbl_size		0x0A




typedef struct {
	pad[0x0C];
} am_hw_gain_t;	// size = 0x0C

typedef struct {
	pad[0x10];
} am_hw_vol_step_t;	// size = 0x10




//am_hw_gain_t		gain_tbl[am_hw_gain_tbl_size];
//am_hw_vol_step_t	vol_tbl[am_hw_vol_tbl_size];



// r0 = 0x25, 0x1B
// r1 = 0,1
// r2 = 1,0
// r3 = 0xF
// arg_0 = volume_step
void * get_gain(UINT32 r0,  R1, R2, R3, arg_0);

#endif // GAIN_H
