/* Windchime
 * 
 * author: Phillip Popp
 * email: popp.phillip@gmail.com
 */

#include "avr/pgmspace.h"
#include "evolve.h"

/* sine wave table lookup for synthesizer */
char wave256[256]  __attribute__ ((aligned(256))) = {
0 , 3 , 6 , 9 , 12 , 15 , 18 , 21 , 24 , 27 , 30 , 33 , 36 , 39 , 42 , 45 , 48 , 51 , 54 , 
57 , 59 , 62 , 65 , 67 , 70 , 73 , 75 , 78 , 80 , 82 , 85 , 87 , 
89 , 91 , 94 , 96 , 98 , 100 , 102 , 103 , 105 , 107 , 108 , 110 , 112 , 113 , 114 , 116 , 
117 , 118 , 119 , 120 , 121 , 122 , 123 , 123 , 124 , 125 , 125 , 126 , 126 , 126 , 126 , 126 , 
127 , 126 , 126 , 126 , 126 , 126 , 125 , 125 , 124 , 123 , 123 , 122 , 121 , 120 , 119 , 118 , 
117 , 116 , 114 , 113 , 112 , 110 , 108 , 107 , 105 , 103 , 102 , 100 , 98 , 96 , 94 , 91 , 
89 , 87 , 85 , 82 , 80 , 78 , 75 , 73 , 70 , 67 , 65 , 62 , 59 , 57 , 54 , 51 , 
48 , 45 , 42 , 39 , 36 , 33 , 30 , 27 , 24 , 21 , 18 , 15 , 12 , 9 , 6 , 3 , 
0 , -3 , -6 , -9 , -12 , -15 , -18 , -21 , -24 , -27 , -30 , -33 , -36 , -39 , -42 , -45 , 
-48 , -51 , -54 , -57 , -59 , -62 , -65 , -67 , -70 , -73 , -75 , -78 , -80 , -82 , -85 , -87 , 
-89 , -91 , -94 , -96 , -98 , -100 , -102 , -103 , -105 , -107 , -108 , -110 , -112 , -113 , -114 , -116 , 
-117 , -118 , -119 , -120 , -121 , -122 , -123 , -123 , -124 , -125 , -125 , -126 , -126 , -126 , -126 , -126 , 
-127 , -126 , -126 , -126 , -126 , -126 , -125 , -125 , -124 , -123 , -123 , -122 , -121 , -120 , -119 , -118 , 
-117 , -116 , -114 , -113 , -112 , -110 , -108 , -107 , -105 , -103 , -102 , -100 , -98 , -96 , -94 , -91 , 
-89 , -87 , -85 , -82 , -80 , -78 , -75 , -73 , -70 , -67 , -65 , -62 , -59 , -57 , -54 , -51 , 
-48 , -45 , -42 , -39 , -36 , -33 , -30 , -27 , -24 , -21 , -18 , -15 , -12 , -9 , -6 , -3 
};


/* notes
 *
 * note_states correspond to specific pitches in a harmonic major scale.  
 */
#define NOTE_STATE_COUNT 14
int current_note_state = 0;
int note_states[NOTE_STATE_COUNT] = {546, 613, 688, 729, 819, 919, 1032, 1093, 1227, 1377, 1459, 1638, 1838, 2063};
byte note_transitions[NOTE_STATE_COUNT][NOTE_STATE_COUNT] = {
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

/* velocities
 * 
 * velocity states correspond to the amplitude of the note.
 */
#define VELOCITY_STATE_COUNT 4
int current_velocity_state = 0;
int velocity_states[VELOCITY_STATE_COUNT] = {8, 16, 32, 48};
byte velocity_transitions[VELOCITY_STATE_COUNT][VELOCITY_STATE_COUNT] = {
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0},
  {0, 0, 0, 0}
};

/* durations
 * 
 * duration states corespond to the length that a note stays on
 */
#define DURATION_STATE_COUNT 12
int current_duration_state = 0;
int duration_states[DURATION_STATE_COUNT] = {36, 72, 96, 108, 120, 132, 144, 156, 178, 288, 576};
byte duration_transitions[DURATION_STATE_COUNT][DURATION_STATE_COUNT] = {
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

/* rest
 * 
 * rest states correspond to the time delay before beginning a new note.
 */
#define REST_STATE_COUNT 12
int current_rest_state = 0;
int rest_states[DURATION_STATE_COUNT] = {18, 36, 48, 60, 72, 84, 96, 18, 120, 18, 144, 156};
byte rest_transitions[REST_STATE_COUNT][REST_STATE_COUNT] = {
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


/* attack,decay are in 1/64ths per 125th of a second - ie. 1 = 0->1 in half a second */
int DECAY=1;
int ATTACK=4;

volatile char* curWave=wave256;

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

// this is supposedly the audio clock frequency - as
// you can see, measured freq may vary a bit from supposed clock frequency
// I'm not quite sure why
// const double refclk=31372.549;  // =16MHz / 510
// const double refclk=31376.6;      // measured


// variables used inside interrupt service declared as voilatile
// these variables allow you to keep track of time - as delay / millis etc. are 
// made inactive due to interrupts being disabled.
volatile unsigned char loopSteps=0; // once per sample
volatile unsigned int loopStepsHigh=0; // once per 256 samples
volatile unsigned int pos = 0;
//volatile int outValue = 0;

// information about the current state of a single oscillator
struct oscillatorPhase {
   unsigned int phaseStep;
   char volume;
   unsigned int phaseAccu;
};

#define OSCILLATOR_COUNT 8
volatile struct oscillatorPhase oscillators[OSCILLATOR_COUNT];
int oscillator_durations[OSCILLATOR_COUNT];
int oscillator_velocities[OSCILLATOR_COUNT];
int rest_duration = 0;

/* this updates the current_note_state, current_velocity_state, and
 * current_duration_state values based off of random numbetrs and 
 * transition matrices */
#define PROB_BASE 1000
#define MUTATION_RATE 5
#define CROSS_RATE 0

int mutate_notes(int val) {
  /* multiply by 2^[-12, 12] / 12 */

  float note = val;
  float original = log(note * 31376.7 / 28835840.0) / log(2.0) * 12.0 + 69;
  original = round(original);
  /*
  if (random(100) > 20) {
    //original += 12.0;
  }
  else { 
    original -= 12.0;
  }
  */
  original += random(-12, 6);
  note = (65536.0 / 31376.6) * pow(2.0, ((original - 69) / 12.0)) * 440.0;
  return (int)note;
}

int mutate_durations(int val) {
  /* double or half */
  int seed = random(100);
  if (seed > 45) {
    val *= 2;
  }
  else {
    val /= 2;
  }
  
  if (val < 4) {
    val = 4;
  }
  return val;
}

int mutate_rests(int val) {
  /* double or half */
  int seed = random(100);
  if (seed > 75) {
    val *= 2;
  }
  else {
    val /= 2;
  }
  
  if (val < 4) {
    val = 4;
  }
  return val;
}

int mutate_velocities(int val) {
  /* add noise & clip */
  val = val + random(-32, 32);
  if (val > 63) {
    val = 63;
  }
  else if (val < 1) {
    val = 1;
  }
  return val;
}

void evolve_array(int *array, int len, mutate_function_t mutate_function) {
  for (int i = 0; i < len; i++) {
    int mutate = random(PROB_BASE) < MUTATION_RATE;
    int cross = random(PROB_BASE) < CROSS_RATE;
  
    if (cross) {
      int loc = random(len);
      int tmp = array[loc];
      array[loc] = array[i];
      array[i] = tmp;
    }  
    
    if (mutate) {
      array[i] = mutate_function(array[i]);
    }
  }
}


void mutate_states() {
  evolve_array(note_states, NOTE_STATE_COUNT, mutate_notes);
  evolve_array(velocity_states, VELOCITY_STATE_COUNT, mutate_velocities);
  evolve_array(duration_states, DURATION_STATE_COUNT, mutate_durations);
  evolve_array(rest_states, REST_STATE_COUNT, mutate_rests);
}

/* this updates the current_note_state, current_velocity_state, and
 * current_duration_state values based off of random numbetrs and 
 * transition matrices */
void update_states() {
	// get next note state first
	int max_prob = 0;
	int rand_val = 0;
	int current_prob = 0;
	mutate_states();

	/* note states control how many notes are played */
	for (int i = 0; i < NOTE_STATE_COUNT; i++) {
		max_prob += note_transitions[current_note_state][i];
	}

	rand_val = random(max_prob);
	for (int i = 0; i < NOTE_STATE_COUNT; i++) {
		current_prob += note_transitions[current_note_state][i];
		if (rand_val < current_prob) {
			current_note_state = i;
			break;
		}
	}

	/* get duration values */
	max_prob = 0;
	current_prob = 0;
	for (int i = 0; i < DURATION_STATE_COUNT; i++) {
		max_prob += duration_transitions[current_duration_state][i];
	}

	rand_val = random(max_prob);
	for (int i = 0; i < DURATION_STATE_COUNT; i++) {
		current_prob += duration_transitions[current_duration_state][i];
		if (rand_val < current_prob) {
			current_duration_state = i;
			break;
		}
	}

	/* get velocity values (only consider velocities that have same number
	 * of notes) */
	max_prob = 0;
	current_prob = 0;
	for (int i = 0; i < VELOCITY_STATE_COUNT; i++) {
		max_prob += velocity_transitions[current_velocity_state][i];
	}

	rand_val = random(max_prob);
	for (int i = 0; i < VELOCITY_STATE_COUNT; i++) {
		current_prob += velocity_transitions[current_velocity_state][i];
		if (rand_val < current_prob) {
			current_velocity_state = i;
			break;
		}
	}

        
        /* get rest value */
	max_prob = 0;
	current_prob = 0;
	for (int i = 0; i < REST_STATE_COUNT; i++) {
	    max_prob += rest_transitions[current_rest_state][i];
	}

	rand_val = random(max_prob);
	for (int i = 0; i < REST_STATE_COUNT; i++) {
		current_prob += rest_transitions[current_rest_state][i];
		if (rand_val < current_prob) {
			current_rest_state = i;
			break;
		}
	}
}

void apply_states() {
  /* updates oscillators with current notes, velocities & durations */
  int note = 0;
  for (int i = 0; i < OSCILLATOR_COUNT; i++) {
    if (oscillators[i].volume < 1) {
      oscillators[i].volume = 0;
      oscillators[i].phaseStep = note_states[current_note_state];
      oscillator_durations[i] = duration_states[current_duration_state];
      oscillator_velocities[i] = velocity_states[current_velocity_state];
      break;
    }
  }

  rest_duration = rest_states[current_rest_state];
}

void randomize_transitions(int count, byte* probs) {
  int minimum = 0;
  int maximum = 256 / count;
  float tightness = random(-1.0, -0.1);
  
  for (int i = 0; i < count; i++) {
    float val = random(-20.0, 5.0);
    val = 1 / (1 + exp(tightness * val));
    probs[i] = (int)(val * maximum);
    if (probs[i] < minimum) {
      probs[i] = minimum;
    }
  }
}

void setup() {
  Serial.begin(9600);        // connect to the serial port
  pinMode(11, OUTPUT);     // pin11= PWM  output / frequency output
  randomSeed(analogRead(0));
  curWave = wave256;
  
  /* initialize oscillators */
  for(int c = 0; c < OSCILLATOR_COUNT; c++) {
    oscillators[c].volume=0;
    oscillators[c].phaseStep=0;
    oscillators[c].phaseAccu=0;
    oscillator_durations[c] = 0;
  }
  
  /* iniialize random attack and decay */
  DECAY = random(1, 8);
  ATTACK = random(1, 16);

  /* initialize various markov model state transitions matrices */
  for (int i = 0; i < NOTE_STATE_COUNT; i++) {
    randomize_transitions(NOTE_STATE_COUNT, note_transitions[i]);
  }
  
  for (int i = 0; i < DURATION_STATE_COUNT; i++) {
    randomize_transitions(DURATION_STATE_COUNT, duration_transitions[i]);
  }
  
  for (int i = 0; i < VELOCITY_STATE_COUNT; i++) {
    randomize_transitions(VELOCITY_STATE_COUNT, velocity_transitions[i]);
  }
  
  for (int i = 0; i < REST_STATE_COUNT; i++) {
    randomize_transitions(REST_STATE_COUNT, rest_transitions[i]);
  }

  apply_states();
  Setup_timer2();
  
  // disable interrupts to avoid timing distortion
  cbi (TIMSK0,TOIE0);              // disable Timer0 !!! delay() is now not available
  sbi (TIMSK2,TOIE2);              // enable Timer2 Interrupt
}


void loop() {
  // we keep a list of 'raw' volumes - and turn down the volume if a chord is taking >64 volume total
  // this is to allow chording without reducing the volume of single notes
  int rawVolumes[OSCILLATOR_COUNT] = {0};
  int curNote = 0;
  unsigned int diffStep = 0;
  unsigned int lastStep = loopStepsHigh;
  unsigned int curStep = loopStepsHigh;

  /* perform endless loop wich updates the oscillator volumes and frequencies.
   * This loop can be considered to be at the "control" rate, as opposed to the
   * "audio" rate.  Maximally, it's called once every 256 samples */
  while(1) {
    curStep=loopStepsHigh;
    if (lastStep > curStep) {
	  /* when the step integers rollover, we get the "lastStep" being greater 
	   * than the current.  To avoid having weird sound artifacts, we set this
	   * the diffStep to 1 in this case
	   */
      diffStep = 1;
    }
    else {
      diffStep = curStep - lastStep;
    }
    
    if (diffStep > 0) {
      lastStep=curStep;
                        
      /* update oscillator velocities and track total volume */
      int totalVolume = 0;
      for(int c = 0; c < OSCILLATOR_COUNT; c++) {
        if (oscillator_durations[c] == 0) {
          /* note is done, so decay */
          rawVolumes[c] -= DECAY * (diffStep);
        }
        else {
          rawVolumes[c] += ATTACK * (diffStep);
        }

        if(rawVolumes[c] > oscillator_velocities[c]) {
          rawVolumes[c] = oscillator_velocities[c];
        }
        else if(rawVolumes[c] < 0) {
          rawVolumes[c] = 0;
        }
        totalVolume += rawVolumes[c];
      }
         
      if (totalVolume >= 64) {
		/* scale down volume if oscillators are going to saturate
		 * the output value 
		 */
        for(int c = 0; c < OSCILLATOR_COUNT; c++) {
          oscillators[c].volume = (rawVolumes[c] * 63) / totalVolume;
        }
	  }
      else {
        for(int c = 0; c < OSCILLATOR_COUNT; c++) {
          oscillators[c].volume = rawVolumes[c];
        }
      }

      /* update oscillator durations */
      for (int c = 0; c < OSCILLATOR_COUNT; c++) {
        oscillator_durations[c] -= diffStep;
        if (oscillator_durations[c] <= 0) {
          /* avoid wrapping around and falsley triggering state
           * transitions */
          oscillator_durations[c] = 0;
        }
      }
      
	  /* update rest durations */
      rest_duration -= diffStep;              
      if (rest_duration <= 0) {
	    /* play a new note! */
        apply_states();
        /* transition markov states and mutate */
        update_states();
      }
    }
  }
}

//******************************************************************
// timer2 setup
// set prscaler to 1, PWM mode to phase correct PWM,  16000000/510 = 31372.55 Hz clock
void Setup_timer2() {
	// Timer2 Clock Prescaler to : 1
	sbi (TCCR2B, CS20);
	cbi (TCCR2B, CS21);
	cbi (TCCR2B, CS22);

	// Timer2 PWM Mode set to Phase Correct PWM
	cbi (TCCR2A, COM2A0);  // clear Compare Match
	sbi (TCCR2A, COM2A1);

	sbi (TCCR2A, WGM20);  // Mode 1  / Phase Correct PWM
	cbi (TCCR2A, WGM21);
	cbi (TCCR2B, WGM22);
}




#define HIBYTE(__x) ((char)(((unsigned int)__x)>>8))
#define LOBYTE(__x) ((char)(((unsigned int)__x)&0xff))

//******************************************************************
// Timer2 Interrupt Service at 31372,550 KHz = 32uSec
// this is the timebase REFCLOCK for the DDS generator
// FOUT = (M (REFCLK)) / (2 exp 32)
// runtime : ?
ISR(TIMER2_OVF_vect) {
    int outValue = 0;
    int tempStep=0;
    char tempPhaseLow=0,tempVolume=0;
    int tempWaveBase=0;
    int pos = 0;

    for (int i = 0; i < OSCILLATOR_COUNT; i++) {
      oscillators[i].phaseAccu += oscillators[i].phaseStep;
      //outValue += oscillators[i].volume * wave256[oscillators[i].phaseAccu & 0x00FF];
      pos = (oscillators[i].phaseAccu & 0xFF00) >> 8;
      //outValue += oscillators[i].volume * wave256[pos];
      outValue += oscillators[i].volume * curWave[pos];
    }
    
    // at this point outValue = oscillator value 
    // it is currently maxed to full volume / 4
    // to allow some headroom for filtering 
    // full gain     
    outValue*=4;

    // at this point, outValue is a 16 bit signed version of what we want ie. 0 -> 32767, then -32768 -> -1 (0xffff)
    // we want 0->32767 to go to 32768-65535 and -32768 -> -1 to go to 0-32767, then we want only the top byte
    // take top byte, then add 128, then cast to unsigned. The (unsigned int) in the below is to avoid having to shift (ie.just takes top byte)     
    char valOut=((unsigned int)(outValue))>>8;
    valOut+=128;

    OCR2A=(byte)valOut;

    // increment loop step counter (and high counter)
    // these are used because we stop the timer
    // interrupt running, so have no other way to tell time
    // this asm is probably not really needed, but it does save about 10 instructions
    // because the variables have to be volatile
    asm(
        "inc %[loopSteps]" "\n\t"
        "brbc 1,loopend%=" "\n\t"
        "inc %A[loopStepsHigh]" "\n\t"
        "brbc 1,loopend%=" "\n\t"
        "inc %B[loopStepsHigh]" "\n\t"
        "loopend%=:" "\n\t"
          :[loopSteps] "+a" (loopSteps),[loopStepsHigh] "+a" (loopStepsHigh):);    
} 
