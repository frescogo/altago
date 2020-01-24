void CEL_Load (void) {
    Serial.println("Load");
}

void CEL_Restart (void) {
    Serial.println("Restart");
}

void CEL_Go (void) {
    Serial.println("GO!");
}

void CEL_Hit (void) {
    Serial.println("Hit");
}

void CEL_Queda (void) {
    Serial.println("Queda");
    Serial_Placar();
}

void CEL_Timeout (void) {
    Serial.println("Timeout");
    Serial_Placar();
}
