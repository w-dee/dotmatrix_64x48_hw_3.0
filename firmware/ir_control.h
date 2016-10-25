#ifndef IR_CONTROL_H
#define IR_CONTROL_H

/**
 * the ir state machine status enum
 */
enum ir_status_t
{
	IR_NONE, //!< idle
	IR_RECORD, //!< recording
	IR_REPLAY, //!< replaying
	IR_SUCCESS, //!< successful end
	IR_FAILED //!< unsucessful end
};

void ir_init();
void ir_record();
void ir_replay();

ir_status_t ir_get_status();

#endif

