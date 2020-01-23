#define MAJOR    0
#define MINOR    1
#define REVISION 0

//#define DEBUG

#ifdef DEBUG
#define assert(x) \
    if (!x) {                               \
        pinMode(LED_BUILTIN,OUTPUT);        \
        while (1) {                         \
            digitalWrite(LED_BUILTIN,HIGH); \
            delay(250);                     \
            digitalWrite(LED_BUILTIN,LOW);  \
            delay(250);                     \
        }                                   \
    }
#else
#define assert(x)
#endif

#include <EEPROM.h>
#include "pitches.h"

#define MAX_HITS   400
#define MAX_DT     8191     // 2^13-1

#define PIN_JOGS   2
#define PIN_CFG    6
#define PIN_TONE   11

typedef enum {
    JOG_NO = -1,
    JOG_A  =  0,
    JOG_B,
    JOG_C,
    JOG_D
} TJOG;

typedef enum {
    CFG_NO = 0,
    CFG_1,
    CFG_2,
    CFG_B,
    CFG_BC
} TCFG;

typedef struct {
    u16 jog :  2;
    u16 cab :  1;
    u16 dt  : 13;
} THIT;

typedef struct {
    u16  hit;
    THIT hits[MAX_HITS];
} Save;
static Save S;

void EEPROM_Load (void) {
    for (int i=0; i<sizeof(Save); i++) {
        ((byte*)&S)[i] = EEPROM[i];
    }
}

void EEPROM_Save (void) {
    for (int i=0; i<sizeof(Save); i++) {
        EEPROM[i] = ((byte*)&S)[i];
    }
}

void EEPROM_Default (void) {
    S.hit = 0;
}

TCFG IN_Cfg (void) {
    static u32  NOW;
    static bool WAS = false;
    static bool RST = false;
    static bool IN2 = false;

    u32  now = millis();
    bool is  = (digitalRead(PIN_CFG) == LOW);
    TCFG ret = CFG_NO;

    // nao estava e nem esta pressionado
    if (!WAS && !is) {
        return CFG_NO;
    }

    // nao estava mas agora esta pressionado
    if (!WAS && is) {
        tone(PIN_TONE, NOTE_C2, 50);
        NOW = now;
        WAS = true;
        return CFG_NO;
    }

    // ja estava pressionado
    assert(WAS);
    WAS = is;

    if (!is) {                      // mas nao esta mais
        IN2 = false;                //   volta a ser possivel o 2
        return CFG_1;               //   entao nem passou o tempo e retorno o 1
    } else {                        // e continua
        if (!IN2 && now-NOW>750) {  //   entao verifico se passou o tempo do 2
            IN2 = true;             //     nao eh mais possivel o 2 ate soltar
            if (digitalRead(PIN_JOGS+JOG_B) == LOW) {
                return CFG_B;
            } else {
                return CFG_2;
            }
        } else {
            return CFG_NO;
        }
    }
}

TJOG IN_Jog (void) {
    if (digitalRead(PIN_JOGS+JOG_A) == LOW) {
        return JOG_A;
    } else if (digitalRead(PIN_JOGS+JOG_B) == LOW) {
        return JOG_B;
    } else if (digitalRead(PIN_JOGS+JOG_C) == LOW) {
        return JOG_C;
    } else if (digitalRead(PIN_JOGS+JOG_D) == LOW) {
        return JOG_D;
    }
    return JOG_NO;
}

bool IN_Jog_Alta (TJOG jog) {
    delay(180);
    return (digitalRead(PIN_JOGS+jog) == LOW);
}

void loop () {};

void setup (void)
{
    Serial.begin(9600);

    pinMode(PIN_CFG        , INPUT_PULLUP);
    pinMode(PIN_JOGS+JOG_A , INPUT_PULLUP);
    pinMode(PIN_JOGS+JOG_B , INPUT_PULLUP);
    pinMode(PIN_JOGS+JOG_C , INPUT_PULLUP);
    pinMode(PIN_JOGS+JOG_D , INPUT_PULLUP);

    EEPROM_Load();
    goto _CONTINUE;

    // JOGOS
    while (1)
    {
_RESTART:
        tone(PIN_TONE, NOTE_C5, 2000);
        S.hit = 0;
        EEPROM_Save();

_CONTINUE:
        // SEQUENCIAS
        while (1)
        {
            // espera apito
            TCFG cfg;
            while ((cfg=IN_Cfg()) < CFG_2);
            switch (cfg) {
                case CFG_B:
                    goto _RESTART;
                case CFG_BC:
                    EEPROM_Default();
                    goto _RESTART;
            }
            assert(cfg == CFG_2);
            tone(PIN_TONE, NOTE_C7, 500);

            TJOG JOG;

            // espera saque
            while ((JOG=IN_Jog()) == JOG_NO);
            tone(PIN_TONE, NOTE_C5, 50);

            // saque
            S.hits[S.hit].jog = JOG;
            S.hits[S.hit].cab = false;
            S.hits[S.hit].dt  = 0;
            S.hit++;

            u32 NOW = millis();

            // TOQUES
            while (1)
            {
                // queda
                if (IN_Cfg() == CFG_2) {
                    goto _FALL;
                }

                // toque
                TJOG jog = IN_Jog();
                if (jog!=JOG_NO && jog!=JOG)
                {
                    S.hits[S.hit].jog = jog;

                    u32  now = millis();
                    u32  dt  = now - NOW;
                    S.hits[S.hit].dt = min(MAX_DT,dt);

                    tone(PIN_TONE, NOTE_C5, 50);        // antes de IN_Jog_Alta

                    bool alta = IN_Jog_Alta(jog);       // apos tone
                    if (alta) {
                        tone(PIN_TONE, NOTE_C4, 30);
                    }
                    S.hits[S.hit].cab = alta;

                    S.hit++;
                    JOG = jog;
                    NOW = now;
                }
            }

_FALL:
            Serial.println("---");
            Serial.println(S.hit);
            Serial.println("---");
            for (int i=0; i<S.hit; i++) {
                Serial.print(S.hits[i].jog);
                Serial.print("  ");
                Serial.print(S.hits[i].cab);
                Serial.print("  ");
                Serial.println(S.hits[i].dt);
            }

            tone(PIN_TONE, NOTE_C4, 100);
            delay(110);
            tone(PIN_TONE, NOTE_C3, 100);
            delay(110);
            tone(PIN_TONE, NOTE_C2, 300);
            delay(310);

            EEPROM_Save();
        }
    }
}
