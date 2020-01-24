void PC_Load (void) {
    Serial.println("Load");
}

void PC_Restart (void) {
    Serial.println("Restart");
}

void PC_Go (void) {
    Serial.println("GO!");
}

void PC_Hit (void) {
    Serial.println("Hit");
}

void PC_Queda (void) {
    Serial.println("Queda");
    Serial_Placar();
}

void PC_Timeout (void) {
    Serial.println("Timeout");
    Serial_Placar();
}
