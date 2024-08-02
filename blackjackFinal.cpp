#include <bits/stdc++.h>
using namespace std;
int rc = 0;//running count
vector<int> shoe;//vector represents a shoe
vector<int> hand;//represents player hand
vector<int> dealer;//represents dealer hand
queue<int> newHands;//represents hands that are yet to be played; reminder that each hand is two cards. consists of trio: how many splits deep the hand is, and the hand.
int os[1000] = {0};//optimized, array version of vectors
int oh[100] = {0};
int od[100] = {0};
int on[3][100] = {0};
double ot[2][100] = {0};
int osi = 0;//index for optimized arrays
int ohi = 0;
int odi = 0;
int oni = 0;
int oti = 0;
vector<pair<int, int>> totals;//stores the totals from however many hands are being played by the player as well as how much they are worth
auto rng = default_random_engine{std::chrono::system_clock::now().time_since_epoch().count()};//for shuffling
int counts[11] = {0, -1, 1, 1, 1, 1, 1, 0, 0, 0, -1};//stores the value of each card
int hs[50][500][200] = {0};//stores whether you should hit or stand
int sp[50][500][200] = {0};//stores whether you should split
int db[50][500][200] = {0};//stores whether you should double
int sr[50][500][200] = {0};//stores whether you should surrender
//rules
int numDeck = 6;//number of decks in a shoe
int penetration = 2;//number of decks under the cut card
int surrenderAllowed = 0;//is surrender allowed?
int doubleAfterSplit = 0;//can you double after a split?
int s17 = 0;//dealer stands on 17?
int resplitAces =  0;//can you resplit aces?
int maxSplits = 1;//what's the maximum number of times you can split?
int maxBet = 13;//what's the table maximum, in terms of betting units?
double blackJackModifier = 1.5;//how much money is won when you have a blackJack; 1.5 for 3 to 2, 1.2 for 6 to 5
int insuranceAllowed = 1;//whether insurance is offered
//simulation parameters
long long int iterations = 1000;//how many times the simulation would be run
double acceptedRisk = 1;//acceptable risk of ruin in %
int goal = 200;//goal in betting units
double bankroll = 100;//bankroll in betting units
//functions
void shuffle();//starts new shoe
int draw();//draws a single card from shoe and updates running count
int split(int a, int b);//returns whether you should split
int doubleDown(int a, int b);//returns whether you should double down
int hitOrStand(int a, int b);//returns whether you should hit or stand. 1 on hit, 0 on stand
pair<int, int> value(vector<int> input);//returns whether it is a soft count, and the current value of the hand if they were to stand.
pair<int, int> simulate(int betSpread[7]);//simulate a player playing by the rules and bet spread; return hands played if won, return 0 if lost, second element returns remaining bankroll
int doShoe(int betSpread[7], double &br);//simulate a single shoe; returns number of hands played
double doRound();//simulate a single round; returns number of bets won or lost
int odH();//optimized do hand
int tc();//returns true count
int doDealer();//simulates a dealer, returns dealer's total
pair<int, int> doHand();//simulate a single hand; returns the value of the hand, number of bets riding on this hand.
int surrender(int a, int b);//returns whether you should surrender
void assignArray(int arr1[7], int arr2[7]);//assigns arr2 to arr1
int main(){
    int maxGain = 0;//maximum money per hand
    int possible = 0;//possible to keep risk of ruin that low?
    int optimal[7] = {0};//optimal betspread
    int optimalFails = 0;//number of failures in optimal betspread
    int current[7] = {0};//current betspread
    pair<int, int> results;//stores the result of a single simulation;
    int totalHands = 0;//total number of hands played in successful iterations
    int totalGain = 0;//total amount of money gained in successful iterations
    double avgGain = 0;
    double riskOfRuin = 0;//calculated risk of ruin
    int minFails = INT_MAX;
    int failures  = 0;//number of failures
    int safest[7] = {0};
    char temp;
    cout << "Number of Decks:";
    cin >> numDeck;
    cout << "Deck penetration (number of decks under cut card):";
    cin >> penetration;
    cout << "Surrender allowed[y/n]:";
    cin >> temp;
    surrenderAllowed = temp == 'y';
    cout << "Double after split allowed[y/n]:";
    cin >> temp;
    doubleAfterSplit = temp == 'y';
    cout << "Dealer stands on all 17[y/n]:";
    cin >> temp;
    s17 = temp == 'y';
    cout << "Resplit aces allowed[y/n]:";
    cin >> temp;
    resplitAces = temp == 'y';
    cout << "Max splits:";
    cin >> maxSplits;
    cout << "Max bet in betting units:";
    cin >> maxBet;
    cout << "Blackjack modifier:";
    cin >> blackJackModifier;
    cout << "Insurance allowed[y/n]:";
    cin >> temp;
    insuranceAllowed = temp == 'y';
    cout << "Accepted risk in %:";
    cin >> acceptedRisk;
    cout << "goal:";
    cin >> goal;
    cout << "bankroll in betting units:";
    cin >> bankroll;
    cout << "iterations:";
    cin >> iterations;
    cout << "\n";
    int spreadDone = 0;
    int totalSpread = 0;
    for (int i = 0; i <= 1; i++){//populate arrays for decision
        for (int j = 4; j <= 30; j++){
            for (int k = 1; k <= 10; k++){
                dealer.clear();
                dealer.push_back(k);
                hs[i][j][k] = hitOrStand(i, j);
                db[i][j][k] = doubleDown(i, j);
                sp[i][j][k] = split(i, j);
                sr[i][j][k] = surrender(i, j);
            }
        }
    }
    for (int i = 0; i <= 1; i++){
        for (int j = 4; j <= 17; j++){
//            cout << j << " ";
            for (int k = 1; k <= 10; k++){
//                cout << db[i][j][k] << " ";
            }
//            cout << "\n";
        }
//        cout << "------\n";
    }
    for (int i = 2; i <= 22; i += 2){
        for (int k = 1; k <= 10; k++){
//            cout << sp[0][i][k] << " ";
        }
//        cout << "\n";
    }
    //cin >> iterations;
//    int tmp[] = {1, 2, 3, 4, 5, 6, 7};
//    for (int i = 0; i < 1000; i++){
//        doShoe(tmp, bankroll);
//    }
//    cin >> iterations;
    int thisOne = 0;
    for (int s1 = 1; s1 <= maxBet; s1++){
        for (int s2 = 1; s2 <= s1; s2++){
            for (int s3 = 1; s3 <= s2; s3++){
                for (int s4 = 1; s4 <= s3; s4++){
                    for (int s5 = 1; s5 <= s4; s5++){
                        for (int s6 = 1; s6 <= s5; s6++){
                            //cout << "here";
                            totalSpread++;
                        }
                    }
                }
            }
        }
    }
    for (int s1 = 1; s1 <= maxBet; s1++){
        for (int s2 = 1; s2 <= s1; s2++){
            for (int s3 = 1; s3 <= s2; s3++){
                for (int s4 = 1; s4 <= s3; s4++){
                    for (int s5 = 1; s5 <= s4; s5++){
                        for (int s6 = 1; s6 <= s5; s6++){
                            cout << "Spread " << ++spreadDone << " of " << totalSpread << "\n";
                            thisOne = 0;
                            int temp[7] =  {1, s6, s5, s4, s3, s2, s1};
                            assignArray(current, temp);
                            totalHands = 0;
                            totalGain = 0;
                            failures = 0;
                            riskOfRuin = 0;
                            for (int i = 0; i < iterations; i++){
                                cout << "\r" << i+1 << ":";
                                results = simulate(current);
                                //cout << results.first << "\n";
                                //cout << results.first << "\n";
                                if (results.first == 0){
                                    failures++;
                                    continue;
                                }
                                totalHands += results.first;
                                totalGain += results.second - bankroll;
                            }
                            cout << failures << "\n";
                            if (failures <= acceptedRisk * iterations / 100){
                                possible = 1;
                                thisOne = 0;
                                assignArray(optimal, current);
                                optimalFails = failures;
                                avgGain = ((double)totalGain)/totalHands;
                                if (avgGain > maxGain) {
                                    assignArray(optimal, current);
                                    maxGain = avgGain;
                                    optimalFails = failures;
                                }
                                continue;
                            } else if (minFails > failures){
                                minFails = failures;
                                assignArray(safest, current);
                            }
                            avgGain = ((double)totalGain)/totalHands;
//                            if (avgGain > maxGain) {
//                                assignArray(optimal, current);
//                                maxGain = avgGain;
//                                optimalFails = failures;
//                            }
                            char x;
                            //cin >> x;
                        }
                    }
                }
            }
        }
    }
    if (possible) {
        cout << "It is possible. Optimum bet spread is as follows:\n";
        for (int i:optimal) cout << i << " ";
        cout << "\n" << "Risk of Ruin:" << ((double)optimalFails)/iterations*100 << "%";
    } else {
        cout << "It is not possible. Minimum risk bet spread is as follows:\n";
        for (int i:safest) cout << i << " ";
        cout << "\n" << "Risk of Ruin:" << ((double)minFails)/iterations*100 << "%";
    }
    return 0;
}
void assignArray(int arr1[7], int arr2[7]){
    for (int i = 0; i < 7; i++) arr1[i] = arr2[i];
}
int draw(){//literally just use it as is instead of calling a function for efficiency
    int ret = shoe.back();
    shoe.pop_back();
    rc += counts[ret];
    return ret;
}
void shuffle(){
    osi = 0;
    for (int i = 0; i < numDeck; i++){
        for (int j = 0; j < 4; j++){
            for (int k = 1; k <= 13; k++){
                os[osi++] = min(k, 10);
            }
        }
    }
    shuffle(os, os + 52 * numDeck, rng);
}
int tc(){
    if (rc == 0) return 0;
    int ret = rc/((52*numDeck-osi)/52);
    return min(ret, 6);
    //to do inline: tc rc == 0 ? 0 : min(ret, rc/((52*numDeck-osi)/52));
}
pair<int, int> value(vector<int> input){
    int aces = 0;
    int sum = 0;
    int sc = 0;
    for (int i:input){
        if (i == 1) aces++;
        sum += i;
    }
    for (int i = 0; i < aces; i++){
        if (sum <= 11){
            sum += 10;
            sc = 1;
            break;
        }
    }
    return {sc, sum};
}
int doShoe(int betSpread[7], double &br){
    //cout << "\n" << br << "\n";
    shuffle();
    osi = 0;
    int otc = 0;
    otc = rc == 0 ? 0 : min(6,(int)(rc/((52*numDeck-osi)/52)));
    rc = 0;
    int bet = betSpread[otc];//bet according to the true count
    int numHands = 0;//number of hands played in the deck
    while (52*numDeck-osi > (numDeck - penetration) * 52 && br > 8 * betSpread[6] && (rc == 0 ? 0 : min(6,(int)(rc/((52*numDeck-osi)/52)))) > -1 && br < goal){//while cut card hasn't come out, you have enough bankroll to bet 8 bets, true count isn't negative, and haven't reached goal yet
        otc = rc == 0 ? 0 : min(6,(int)(rc/((52*numDeck-osi)/52)));
        //cout << osi << endl;
        bet = betSpread[otc];//bet size
        double winnings = doRound();//play round
        br += winnings * bet;//update bankroll
        numHands++;
    }
    //cout << br << "\n";
    return numHands;
}
pair<int,int> simulate(int betSpread[7]){
    double br = bankroll;
    int sum = 0;
    int ret = 0;
    while (br > 8 * betSpread[6] && br < goal){//while not successful or broke
        sum += doShoe(betSpread, br);
    }
    if (br >= goal) ret = sum;
    //cout << ret << "\n";
    return {ret, br};
}
double doRound(){
    int foo = 0;
    ohi = 0;
    oni = 0;
    totals.clear();//clear the totals array
    oti = 0;
    double ret = 0;//this will store how many bets are won.
    dealer.clear();//dealer hand is cleared
    odi = 2;
    int nn = 1;//number of hands yet to be played
    on[0][oni] = 0;//deal player his hands
    on[1][oni] = os[osi++];
    rc += counts[on[1][oni]];
    od[0] = os[osi++];
    rc += counts[od[0]];
    on[2][oni] = os[osi++];
    rc += on[2][oni];//player's second hand
    rc += counts[on[1][oni]];
    od[1] = os[osi++];
    rc += counts[od[1]];
    pair<int, int> result;
    int dealerTotal = 0;
    int dealerRan = 0;
    int f = 0;
    int s = 0;
    int splits = 0;
    while(oni < nn){//while there are hands to play
        f = 0;
        s = 0;
        ohi = 2;
        splits = on[0][oni];//how many splits deep are we in?
        oh[0] = on[1][oni];
        oh[1] = on[2][oni];
        s = oh[0] + oh[1]; //sum player's total. that is s. if count is soft, f is 1.
        if (oh[0] == 1 || oh[1] == 1) {
            s += 10;
            f = 1;
        }
        if (!splits && s == 21){//when player gets blackjack; can only happen before a split
            if (od[0] + od[1] == 21) return 0;//if the hand is a push
            return blackJackModifier;//if player won
        }
        if (!splits && od[0] == 1 && insuranceAllowed){//buy insurance?
            if (rc == 0 ? 0 : min(6, (int)(rc/((52*numDeck-osi)/52))) >= 3) {//only take insurance if tc is above 3
                ret -= 0.5;//insurance is half the cost of the bet
                if (od[1] == 10) return 0;//if the insurance works
            }
        }
        if (!splits && surrenderAllowed && sr[f][s][od[0]]){//when player surrenders if possible; can only happen before a split
                return -0.5;//if surrendered
        }
        if (!splits || (doubleAfterSplit)){//if hasn't split yet or double after split is offered
            if (db[f][s][od[0]]) {//if strategy is to double down
                oh[2] = os[osi++];
                rc += counts[oh[2]];
                for (int i = 0; i < 2; i++) {
                    ot[0][oti] = s;
                    ot[1][oti++] = 2;
                }
                oni++;
                nn++;
                continue;
            }
        }
        if (oh[0] == oh[1] && splits < maxSplits && sp[f][s][od[0]] && !(splits != 0 && !resplitAces)){//if logic says to split and we can still split according to the rules
            on[0][oni+1] = splits + 1;
            on[1][oni+1] = oh[1];
            on[2][oni+1] = os[osi++];
            rc += counts[on[2][oni+1]];
            oh[1] = os[osi++];//optimized shoe always increments
            rc += counts[oh[1]];
        }
        ot[0][oti] = odH();
        ot[1][oti] = 1;
        oti++;
        oni++;
    }
    int dsc = 0;//stores whether dealer has a soft count
    for (int i = 0; i < oti; i++){
        //cout << ":" << ot[0][i] << "\n";
        if (ot[0][i] > 21){
            ret -= ot[1][i];
            continue;
        }
        if (!dealerRan) {
            int val = od[0] + od[1];
            while (val <= 17){
                val = 0;
                dsc = 0;
                for (int i = 0; i < odi; i++){
                    val += od[i];
                }
                for (int i = 0; i < odi; i++){
                    if (od[i] == 1 && val <= 11) {
                        val += 10;
                        dsc = 1;
                    }
                }
                if (val == 17){//if 17
                    if (s17) return 17;//stand if rules dictate
                    if (dsc) {//hit if hits soft 17
                        od[odi] = os[osi++];
                        rc += counts[od[odi++]];
                    } else break;//stands if hard 17
                } else {
                    od[odi] = os[osi++];
                    rc += counts[od[odi++]];
                }
            }
            dealerTotal = val;
        }
        //cout << dealerTotal << ":" << ot[0][i] << "\n";
        //cout << ot[0][i] << ":" << dealerTotal << "\n";
        if (dealerTotal > 21) ret += ot[1][i];
        else if (ot[0][i] > dealerTotal) ret += ot[1][i];
        else if (ot[0][i] < dealerTotal) ret -= ot[1][i];
        else ret = ret;
    }
    //cout << ret << "\n";
    return ret;
}
int split(int a, int b){
    pair<int, int> res = {a, b};
    switch (res.second){
        case 2:
            return 1;
            break;
        case 4:
        case 6:
            if (dealer[0] >= 4 && dealer[0] <= 6) return 1;
            if (doubleAfterSplit && (dealer[0] == 2 || dealer[0] == 3)) return 1;
            break;
        case 8:
            if (doubleAfterSplit){
                if (dealer[0] == 5 || dealer[0] == 6) return 1;
            }
            break;
        case 10:
            break;
        case 12:
            if (dealer[0] <= 6 && dealer[0] >= 3) return 1;
            break;
        case 14:
            if (dealer[0] >= 2 && dealer[0] <= 7) return 1;
            break;
        case 16:
            return 1;
            break;
        case 18:
            if (dealer[0] == 7 || dealer[0] == 10 || dealer[0] == 1) return 0;
            else return 1;
            break;
        case 20:
            break;
        case 22:
            return 1;
 
    }
    return 0;
}
pair<int, int> doHand(){
    int bets = 1;
    int decision = 0;
    pair<int, int> val = value(hand);
    int first = val.first;
    int second = val.second;
    while (1){
        val = value(hand);
        first = val.first;
        second = val.second;
        decision = hs[first][second][dealer[0]];
        if (decision == 0) break;
        hand.push_back(draw());
    }
    return {value(hand).second, bets};
}
int odH(){//returns value of hand at end of play
    ohi = 2;
    int decision = 0;
    int f, s;
    f = s = 0;
    while (1){ //sum player's total. that is s. if count is soft, f is 1.
        s = 0;
        f = 0;
        for (int i = 0; i < ohi; i++) {
          //  cout << "/" << oh[i];
            s += oh[i];
        }
        //cout << "\n";
        //cout << s;
        for (int i = 0; i < ohi; i++) {
            if (s <= 11 && oh[i] == 1){
                s += 10;
                f = 1;
                break;
            }
        }
        decision = hs[f][s][od[0]];
        //cout << ":" << os[osi] << endl;
        //cout << decision << ":" << f << ":" << s << ":" << od[0] << endl;
        if (decision == 0 || s >= 21) break;
        oh[ohi] = os[osi++];
        rc += counts[oh[ohi++]];
    }
    return s;
}
int hitOrStand(int a, int b){
    pair<int, int> res = {a, b};
    if (res.first){//if the count is soft
        switch(res.second){
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
                return 1;
                break;
            case 18:
                if (dealer[0] >= 9 || dealer[0] == 1) return 1;
                break;
            default:
                break;
        }
    } else {
        if (res.second > 21) return 0;
        switch (res.second){
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                return 1;
                break;
            case 12:
                if (dealer[0] != 4 && dealer[0] != 5 && dealer[0] != 6) return 1;
                break;
            case 13:
            case 14:
            case 15:
            case 16:
                if (dealer[0] >= 2 && dealer[0] <= 6) return 0;
                else return 1;
                break;
            case 17:
                break;
            case 18:
                break;
            case 19:
                break;
            case 20:
                break;
            case 21:
                break;
        }
    }
    return 0;
}
int doubleDown(int a, int b){
    pair<int, int> res = {a, b};
    if (res.first){//if the count is soft
        switch (res.second){
            case 12:
                break;
            case 13:
            case 14:
                if (dealer[0] == 5 || dealer[0] == 6) return 1;
                break;
            case 15:
            case 16:
                if (dealer[0] >= 4 && dealer[0] <= 6) return 1;
                break;
            case 17:
            case 18:
                if (dealer[0] >= 3 && dealer[0] <= 6) return 1;
                if (!s17 && dealer[0] == 2) return 1;
                break;
            case 19:
                if (dealer[0] == 6 && !s17) return 1;
                break;
            case 20:
                break;
            default:
                break;
        }
    } else {
        if (res.second > 21) return 0;
        switch (res.second){
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                if (dealer[0] == 3 || dealer[0] == 4 || dealer[0] == 5 || dealer[0] == 6) return 1;
                break;
            case 10:
                if (dealer[0] == 10 || dealer[0] == 1) return 0;
                else return 1;
                break;
            case 11:
                if (dealer[0] == 1 && s17) return 0;
                else return 1;
                break;
            case 12:
                break;
            case 13:
                break;
            case 14:
                break;
            case 15:
                break;
            case 16:
                break;
            case 17:
                break;
            case 18:
                break;
            case 19:
                break;
            case 20:
                break;
            case 21:
                break;
        }
    }
      return 0;
}
int surrender(int a, int b){
    pair<int, int> res = {a, b};
    if (res.first){//if the count is soft
        switch (res.second){
            case 12:
                break;
            case 13:
                break;
            case 14:
                break;
            case 15:
                break;
            case 16:
                break;
            case 17:
                break;
            case 18:
                break;
            case 19:
                break;
            case 20:
                break;
            default:
                break;
        }
    } else {
        switch (res.second){
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
            case 11:
                break;
            case 12:
                break;
            case 13:
                break;
            case 14:
                break;
            case 15:
                if (dealer[0] == 10) return 1;
                if (dealer[0] == 1 && !s17) return 1;
                break;
            case 16:
                if (dealer[0] == 9 || dealer[0] == 10 || dealer[0] == 1) return 1;
                break;
            case 17:
                if (dealer[0] == 1 && !s17) return 1;
                break;
            case 18:
                break;
            case 19:
                break;
            case 20:
                break;
            case 21:
                break;
        }
    }
    return 0;
}
int doDealer(){
    int val = value(dealer).second;
    while (val <= 17){
        if (val == 17){//if 17
            if (s17) return 17;//stand if rules dictate
            if (value(dealer).first == 1) dealer.push_back(draw());//hit if hits soft 17
            else break;//stands if hard 17
        } else dealer.push_back(draw());
        val = value(dealer).second;//update dealer's hand value
    }
    return val;//return dealer's hand value
}
