#include <bits/stdc++.h>
using namespace std;
int rc = 0;//running count
vector<int> shoe;//vector represents a shoe
vector<int> hand;//represents player hand
vector<int> dealer;//represents dealer hand
queue<int> newHands;//represents hands that are yet to be played; reminder that each hand is two cards. consists of trio: how many splits deep the hand is, and the hand.
vector<pair<int, int>> totals;//stores the totals from however many hands are being played by the player as well as how much they are worth
auto rng = default_random_engine{};//for shuffling
int counts[11] = {0, -1, 1, 1, 1, 1, 1, 0, 0, 0, -1};//stores the value of each card
//rules
int numDeck = 6;//number of decks in a shoe
int penetration = 2;//number of decks under the cut card
int surrenderAllowed = 0;//is surrender allowed?
int doubleAfterSplit = 0;//can you double after a split?
int s17 = 0;//dealer stands on 17?
int resplitAces =  0;//can you resplit aces?
int maxSplits = 1;//what's the maximum number of times you can split?
int maxBet = 5;//what's the table maximum, in terms of betting units?
double blackJackModifier = 1.5;//how much money is won when you have a blackJack; 1.5 for 3 to 2, 1.2 for 6 to 5
int insuranceAllowed = 1;//whether insurance is offered
//simulation parameters
long long int iterations = 100;//how many times the simulation would be run
double acceptedRisk = 1;//acceptable risk of ruin in %
int goal = 2000;//goal in betting units
double bankroll = 1000;//bankroll in betting units
//functions
void shuffle();//starts new shoe
int draw();//draws a single card from shoe and updates running count
int split();//returns whether you should split
int doubleDown();//returns whether you should double down
int hitOrStand();//returns whether you should hit or stand. 1 on hit, 0 on stand
pair<int, int> value(vector<int> input);//returns whether it is a soft count, and the current value of the hand if they were to stand.
pair<int, int> simulate(int betSpread[7]);//simulate a player playing by the rules and bet spread; return hands played if won, return 0 if lost, second element returns remaining bankroll
int doShoe(int betSpread[7], int &br);//simulate a single shoe; returns number of hands played
double doRound();//simulate a single round; returns number of bets won or lost
int tc();//returns true count
int doDealer();//simulates a dealer, returns dealer's total
pair<int, int> doHand();//simulate a single hand; returns the value of the hand, number of bets riding on this hand.
int surrender();//returns whether you should surrender
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
//    cin >> numDeck;
    cout << "Deck penetration (number of decks under cut card):";
 //   cin >> penetration;
    cout << "Surrender allowed[y/n]:";
//    cin >> temp;
//    surrenderAllowed = temp == 'y';
    cout << "Double after split allowed[y/n]:";
//    cin >> temp;
//    doubleAfterSplit = temp == 'y';
    cout << "Dealer stands on all 17[y/n]:";
//    cin >> temp;
//    s17 = temp == 'y';
    cout << "Resplit aces allowed[y/n]:";
//    cin >> temp;
//    resplitAces = temp == 'y';
    cout << "Max splits:";
//    cin >> maxSplits;
    cout << "Table max in betting units:";
//    cin >> maxBet;
    cout << "Blackjack modifier:";
//    cin >> blackJackModifier;
    cout << "Insurance allowed[y/n]:";
//    cin >> temp;
//    insuranceAllowed = temp == 'y';
    cout << "Accepted risk in %:";
//    cin >> acceptedRisk;
    cout << "goal:";
//    cin >> goal;
    cout << "bankroll:";
//    cin >> bankroll;
    cout << "iterations:";
//    cin >> iterations;
    cout << "\n";
    int spreadDone = 0;
    int totalSpread = 1;
    for (int i = 1; i <= maxBet; i++) totalSpread *= i;
    for (int s1 = 1; s1 <= maxBet; s1++){
        for (int s2 = 1; s2 <= s1; s2++){
            for (int s3 = 1; s3 <= s2; s3++){
                for (int s4 = 1; s4 <= s3; s4++){
                    for (int s5 = 1; s5 <= s4; s5++){
                        for (int s6 = 1; s6 <= s5; s6++){
                            cout << "Spread " << ++spreadDone << " of " << totalSpread << "\n";
                            int temp[7] =  {1, s6, s5, s4, s3, s2, s1};
                            assignArray(current, temp);
                            totalHands = 0;
                            totalGain = 0;
                            failures = 0;
                            riskOfRuin = 0;
                            for (int i = 0; i < iterations; i++){
                                cout << i+1 << "\n";
                                results = simulate(current);
                                if (results.first == 0){
                                    continue;
                                }
                                totalHands += results.first;
                                totalGain += results.second - bankroll;
                            }
                            if (failures <= iterations/100*acceptedRisk) possible = 1;
                            else if (minFails > failures){
                                minFails = failures;
                                assignArray(safest, current); 
                            }
                            avgGain = ((double)totalGain)/totalHands;
                            if (avgGain > maxGain) {
                                assignArray(optimal, current);
                                maxGain = avgGain;
                                optimalFails = failures;
                            }
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
int draw(){
    int ret = shoe.back();
    shoe.pop_back();
    rc += counts[ret];
    return ret;
}
void shuffle(){
    shoe.clear();
    for (int h = 0; h < numDeck; h++){
        for (int i = 0; i < 4; i++){
            for (int j = 1; j <= 13; j++){
                shoe.push_back(min(j,10));
            }
        }
    }
    shuffle(shoe.begin(), shoe.end(), rng);
    rc = 0;
}
int tc(){
    if (rc == 0) return 0;
    int ret = rc/(shoe.size()/52);
    return min(ret, 6);
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
    shuffle();
    int bet = betSpread[tc()];//bet according to the true count
    int numHands = 0;//number of hands played in the deck
    while (shoe.size() > (numDeck - penetration) * 52 && br > 8 * betSpread[6] && tc() > -1 && br < goal){//while cut card hasn't come out, you have enough bankroll to bet 8 bets, true count isn't negative, and haven't reached goal yet
        bet = betSpread[tc()];//bet size
        double winnings = doRound();//play round
        br += winnings * bet;//update bankroll
        numHands++;
    }
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
    return {ret, br};
}
double doRound(){
    totals.clear();//clear the totals array
    double ret = 0;//this will store how many bets are won.
    dealer.clear();//dealer hand is cleared
    newHands.push(0);//indicating the new hand is 0 splits deep
    newHands.push(draw());
    dealer.push_back(draw());
    newHands.push(draw());
    dealer.push_back(draw());
    int splits = 0;//variable to store how deep we are in splits
    pair<int, int> result;
    int dealerTotal = 0;
    int dealerRan = 0;
    while(newHands.size() > 0){//while there are no more hands to play
        hand.clear();//clear the hand
        splits = newHands.front();//how many splits deep are we in?
        newHands.pop();
        hand.push_back(newHands.front());
        newHands.pop();
        hand.push_back(newHands.front());
        newHands.pop();
        if (!splits && value(hand).second == 21){//when player gets blackjack; can only happen before a split
            if (value(dealer).second == 21) return 0;//if the hand is a push
            return blackJackModifier;//if player won
        }
        if (!splits && dealer[1] == 1 && insuranceAllowed){//buy insurance?
            if (tc() >= 3) {//only take insurance if tc is above 3
                ret -= 0.5;//insurance is half the cost of the bet
                if (dealer[2] == 10) return 0;//if the insurance works
            }
        }
        if (!splits && surrenderAllowed && surrender()){//when player surrenders if possible; can only happen before a split
                return -0.5;//if surrendered
        }
        if (!splits || (doubleAfterSplit)){//if hasn't split yet or double after split is offered
            if (doubleDown()) {//if strategy is to double down
                hand.push_back(draw());
                result = {value(hand).second, 2};
                totals.push_back(result);
            }
        }
        if (splits < maxSplits && split() && !(splits != 0 && !resplitAces)){//if logic says to split and we can still split according to the rules
            newHands.push(splits + 1);
            newHands.push(hand[1]);
            hand.pop_back();
            hand.push_back(draw());
            newHands.push(draw());
        }
        totals.push_back(doHand());
    }
    for (pair<int, int> i:totals){ //calculate wins/losses
        if (i.first > 21) {//if player busts
            ret -= i.second;
            continue;
        }
        if (!dealerRan) dealerTotal = doDealer();//dealer only plays if no players have bust
        dealerRan = 1;
        if (dealerTotal > 21 || dealerTotal < i.first) ret += i.second;//if dealer busts or player won
        else if (dealerTotal > i.first) ret -= i.second;//if player lost
        else ret = ret;//if push
    }
    return ret;
}
int split(){
    pair<int, int> res = value(hand);
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
        
    }
    return 0;
}
pair<int, int> doHand(){
    int bets = 1;
    int decision = 0;
    while (1){
        decision = hitOrStand();
        if (!decision) break;
        hand.push_back(draw());
    }
    return {value(hand).second, bets};
}
int hitOrStand(){
    pair<int, int> res = value(hand);
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
                if (dealer[0] >= 2 && dealer[0] <= 6) return 1;
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
int doubleDown(){
    pair<int, int> res = value(hand); 
    if (res.first){//if the count is soft
        switch (res.second){
            case 12:
                break;
            case 13:
            case 14:
                if (dealer[0] == 5 || dealer[0] == 6) return 1;
                break;
            case 15:
                break;
            case 16:
                if (dealer[0] >= 4 && dealer[0] <= 6) return 1;
                break;
            case 17:
                break;
            case 18:
                if (dealer[0] >= 4 && dealer[1] <= 6) return 1;
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
int surrender(){
    pair<int, int> res = value(hand);
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
    int val = value(dealer).second;//stores value of dealer's hand
    while (val <= 17){//dealer always hits below a 17
        if (val == 17){//if 17
            if (s17) return 17;//stand if rules dictate
            if (value(dealer).first == 1) dealer.push_back(draw());//hit if hits soft 17
            else break;//stands if hard 17
        } else dealer.push_back(draw());
        val = value(dealer).second;//update dealer's hand value
    }
    return val;//return dealer's hand value
}
