static char STR[64];

void Serial_Placar (void) {
    //Serial.print(F("TOTAL .............. "));
    //sprintf_P(STR, PSTR("%02d.%02d"), G.total/100, G.total%100);
    //Serial.print(STR);
    //Serial.println(F(" pts"));

    Serial.print(F("Restante ..... "));
    int restante = (S.timeout - G.tempo) / 1000;
    sprintf_P(STR, PSTR("%02d:%02d"), restante/60, restante%60);
    Serial.println(STR);

    Serial.print(F("Quedas ....... "));
    Serial.println((int)G.quedas);

    Serial.print(F("Toques ....... "));
    Serial.println((int)G.toques);

    Serial.print(F("Ritmo ........ "));
    if (G.tempo > 5000) {
        Serial.print((int)G.ritmo);
        Serial.println(F(" toqs/min"));
    } else {
        Serial.println(F("---"));
    }

    Serial.println("---");
    Serial.println(S.toq);
    Serial.println("---");
    for (int i=0; i<S.toq; i++) {
        Serial.print(S.toqs[i].jog);
        Serial.print("  ");
        Serial.print(S.toqs[i].cab);
        Serial.print("  ");
        Serial.println(S.toqs[i].dt);
    }
}
