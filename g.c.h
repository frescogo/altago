void G_All (void) {
    G.tempo  = 0;
    G.toques = 0;
    G.quedas = 0;

    for (int i=0 ; i<S.toq ; i++) {
        if (i>0 && S.toqs[i].dt==0) {
            G.quedas++;
        }

        if (i>=S.toq-2 || S.toqs[i+1].dt==0 || S.toqs[i+2].dt==0) {
            // ignora os 2 toques antes das quedas
        } else {
            G.tempo += S.toqs[i].dt;
            G.toques++;
        }
    }

    G.tempo = min(G.tempo, S.timeout);
    G.ritmo = ((u32)G.toques)*6000 / (G.tempo/1000);
}
