
// this is an abstract class, meaning, we cannot write "new Game(...)".
// this is because the class is missing the "simluateGame" method. 
// that method is declared "abstract", meaning "our child classes will have to 
// fill in what it does". 
/** Represents a casino game. */
abstract class Game {
    // every game is required to store a name
    private _name: string;

    // every game has a betting book. the betting book is a hashmap that 
    // maps each player to how much money they are betting.
    private _book: Map< Gambler, number >; 
    // note: there's a design problem here. what if we have a game
    // that needs more betting information? this is actually an example where
    // inheritance isn't ideal. don't worry about it for now while we're 
    // just learning.

    // the casino the game belongs to.
    // we could pass the casino in as an argument, along with book.
    // there are good benefits to that design, but let's do it the more
    // object-oriented, less functional way for our own education:
    private _casino: Casino;

    public get name(): string { return this._name }

    // to construct a game, you have to give it a name
    /** Construct a casino game with the given name, belonging to the
     * given casino.
     */
    constructor( name: string, casino: Casino ) {
        this._name = name;
        this._book = new Map();
        this._casino = casino;
    }
    // but wait, I thought we couldn't construct a Game?
    // we can't, we aren't allowed to write new Game(...);
    // but we are allowed to write new Blackjack(...), and Blackjack is a kind 
    // of game. So Blackjack's constructor will call Game's constructor 
    // (using the super keyword)


    // this method is abstract. each game will do it differently. 
    // however, because it's here, each game *has* to fill in its code. 
    // this means it's safe to have code like this:
    // function something(game: Game) {
    //      ...
    //      game.playGame();
    //      ...   
    // }
    // we can then write something(blackjack) and it will work, because
    // blackjack is a Game, so it can be passed into game, and we know
    // we can call "playGame" on it.
    
    /** Actually run the game and return who won. */
    protected abstract simulateGame(): Gambler[];
    // the child class will figure out which gamblers won and return them.
    // this method is abstract: it has no definition. It's up to the child
    // classes to decide what it does. 


    /**
     * This method tells us how much money a particular person will win.
     * By default, we just return 2x the bet. However, in some games, 
     * how much we return depends on how the gambler bet. Note that none 
     * of the games actually double the player's money, so you might want
     * to pick a different value here.
     * @returns How much to multiply the winnings by
     */
    protected profitMultiplier( _gambler: Gambler ): number { return 2; }

    // this method is *not* abstract, but it calls an abstract method.
    // yes: non-abstract methods can call abstract methods in the same class.
    /** Play the game and give the winners their moeney.
     * Prints all the winners. Removes all elements of this.book. 
     * Updates the casino's profits and losses.
     */
    public playGame(): void {
        console.log( "playing", this.name, "with book:" );
        for( let [player, bet] of this._book ) {
            console.log( "  ", player.name, ": $", bet );
        }

        const winners = this.simulateGame();

        console.log( "game finished!" );

        // For each winner, calculate how much money they won and give it to
        // them. Deduct that much money from the casino.
        for( let winner of winners ) {
            const bet = this._book.get( winner )!;
            const winnings = bet * this.profitMultiplier( winner );
            winner.addMoney( winnings );
            this._casino.addProfit( -winnings );
            console.log( 
                " ", winner.name, "is a winner! they won: ", winnings );

            // remove winners from the book so that only losers will remain.
            this._book.delete( winner );
        }

        // For each loser, take their money and give it to the casino.
        for( let [loser, bet] of this._book ) {
            console.log( " ", loser.name, "has lost!" );
            loser.addMoney( -bet ); // subtract money from losers;
            casino.addProfit( bet ); // give it to the casino

            // also remove losers. the book will be empty after calling 
            // playGame
            // Note: it might be nice to make a functional version of 
            // this where the book is an argument to the method
            // IRL I think this design would be
            // nicer, but it will be more obvious why when you take 
            // programming language design and learn about functional 
            // programming.
            this._book.delete( loser );
        }
    }

    // this function is *not* abstract. We are filling in it's code right now.
    // the child classes will not override this method. It will do the same
    // thing on each child class, so they do not provide their own version of.
    /**
     * Add a player to the game.
     * @param g The gambler to add to the game.
     * @param bet The amount they are betting.
     */
    public addPlayer( g: Gambler, bet: number ): void {
        this._book.set( g, bet );
        // you might wonder why we need a method for this? aren't we just
        // doing one line of code? yes, and many programmers will choose to 
        // avoid this function. one reason to have the function, however, is  
        // that it makes it easier to do more stuff when we add a player 
        // (i.e., logging it to a file somewhere). However, this flexibility
        // comes at the cost of a little bit of complexity. 
    }

    /** Returns a list of people playing the game. */
    public getPlayers(): Gambler[] {
        // this.book.keys() returns an iterator, which is an object that 
        // allows us to scan over a collection using a for loop. We use
        // Array.from(...) to scan over the iterator and add its elements
        // into an array.
        return Array.from(this._book.keys());
    }
}

/** This is a game where the players all place their bets at the same 
 * time. The dealer will flip a coin. If the coin is heads, the players 
 * win and their money is doubled. Otherwise, the players lose their bets. */ 
class TailsIWin extends Game {
    // You need to add a constructor. What should go in it?

    // try commenting out this method and see what error you get.
    // why do you get that error?
    override simulateGame(): Gambler[] {
        throw new Error( "YOUR CODE HERE" );
    }

    // Is the default profitMultiplier behavior okay? 
}


/**
 * Helper function to generate uniform random numbers between [0, upper).
 * So randomInt( 5 ) generates a number between 0 and 4.
 * @param upper The exclusive upper bound (i.e., the number generated will be
 * at most one less than this number)
 * @returns A randum number in the range [0, upper)
 */
function randomInt( upper: number ) {
    // Math.random() goes between 0 and 1, but never hits exactly 1
    return Math.floor( Math.random() * upper );
}

/// This is a game where each player randomly picks a number from 0 to 4.
/// If the dealer  
/**
 * This is a game where each player randomly picks a number from 0 to 4
 * (inclusive). The dealer also picks a number from 0 to 4. If a player
 * picks the same number as the dealer, they get back 4.5x their bet.
 * (total profit of 3.5x). Otherwise, they lose their money.
 */
class GuessTheNumber extends Game {
    
}

/**
 * Simulated guinea-pig racing. Players choose a pig from 0 to 3.
 * Pig #0 has a 50% chance of winning, and pays out 1.9 if they win. 
 * Pig #1 has a 25% chance of winning, and pays out 3.8 if they win.
 * Pig #2 has a 12.5% chance of winning, and pays out 7.6 if they win.
 * Pig #3 has a 12.5% chance of winning, and pays out 7.6 if they win.
 * 
 * There are no complicated horse-racing-style bets (e.g., place, show, etc.),
 * each player just picks a pig. 
 */
class OffTrackGuineaPigRacing extends Game {
    
}

abstract class Gambler {
    private _name: string;
    private _money: number;

    /** how much money the gambler is trying to get */
    private _target: number; 

    public constructor( 
        name: string, 
        startingFunds: number, 
        targetFunds: number 
    ) {
        throw new Error( "YOUR CODE HERE" )
    }

    // These are properties. 
    // When we create a gambler: const gambler = new Gambler(...);
    // we can write this: console.log( gambler.name )
    // get name(): ... makes it so that when we access gambler.name, 
    // the function { return this._name } gets called. This allows us
    // to read the name inside the gambler. 
    // Getters are public by default, so this is a way of reading a public 
    // variable.
    // However, get can only get a value. It's not able to set values. So
    // name is a read-only property, which is what we want. 
    get name(): string { return this._name }
    get money(): number { return this._money }
    get target(): number { return this._target }

    /**
     * Add or deduct a given amount of money to the gambler's bankroll. 
     * @param amount The amount of money to add. Negative means to remove.
     */
    addMoney( amount: number ): void {
        throw new Error( "YOUR CODE HERE" )
    }

    /**
     * @returns Whether the gambler has hit their target.
     */
    public hitTarget(): boolean { throw new Error( "YOUR CODE HERE" ) }


    /**
     * @returns Whether the gambler has run out of money.
     */
    public bankrupt(): boolean { throw new Error( "YOUR CODE HERE" ) }
    
    /**
     * @returns Whether the gambler is finished (i.e., if they've run out
     * of money or have reached their target.)
     */
    public isFinished(): boolean { 
        throw new Error( "YOUR CODE HERE" )
    }

    /**
     * @returns How much the gambler is going to bet next.
     */
    public abstract getBetSize(): number;
}

/**
 * The stable gambler always bets the same amount as long as they have enough
 * money. If they don't, they bet what they have. Their goal is to double 
 * their starting funds.
 */
class StableGambler extends Gambler {
    private _bet: number; 

    public constructor( 
        name: string, 
        startingFunds: number, 
        stableBet: number
    ) {
        super( name, startingFunds, startingFunds * 2 );
        this._bet = stableBet;
    }

    public getBetSize(): number {
        throw new Error( "YOUR CODE HERE" )
    }
}

/**
 * The high risk gambler always bets half of their current money. If they have
 * less than yoloAmount, they bet the remainder of their money. Their goal is
 * to make 5 times their starting amount of money. 
 */
class HighRiskGambler extends Gambler {
    /** if the gambler has <= this amount of money, they bet it all. */
    private _yoloAmount: number;

    /**
     * @param yoloAmnt If the gambler has <= this amount of money, they
     * bet everything they have remaining.
     */
    public constructor(
        name: string,
        startingFunds: number, 
        yoloAmnt: number 
    ) {
        throw new Error( "YOUR CODE HERE" );
    }
}

/**
 * The streak better always increases their bet whenever they win by a 
 * given multiple, and reduces their bet by a given multiple when they lose.
 * For example, if the win multiple is 2.0 and lose multiple is 0.5, the 
 * streak better will double their money when they win and halve it when they
 * lose. You can also do the reverse, making them more conservative when 
 * they win. They start at a given initial bet. 
 * 
 * How do we detect whether we won or lost? Override the addMoney method.
 */
class StreakGambler extends Gambler {
    // YOUR CODE HERE
}


class Casino {
    /** a list of games offered in the casino */
    private _games: Game[];      

    /** a set of guests to the casino */
    private _gamblers: Set<Gambler>;

    /** how much money the casino made today */
    private _profits: number; 

    /** the maximum number of rounds to play */
    private _maxRounds: number;
    private _currentRound: number;

    public constructor( maxRounds: number ) {
        this._games = [
            new TailsIWin( this ),
            new GuessTheNumber( this ),
            new OffTrackGuineaPigRacing( this ),
        ];

        this._profits = 0;

        this._gamblers = new Set([
            // Argument 2 is the amount they start with, 
            // Arg 3 is how much they bet
            new StableGambler( "Alice", 100, 15 ),

            // Argument 2 is the amount they start with
            // Arg 3 is how much they start betting
            // the target is to make 5 times their starting balance, but 
            // you don't see that here because it's calculated inside the 
            // constructor instead of being passed as an argument.
            new HighRiskGambler( "Bob", 50, 10 ),

            // Arg 4 is the minimum amount they will bet 
            // Arg 5 is how much they multiply their bet by when they win
            // Arg 6 is how much they multiply their bet by when they lose
            // Arg 7 is their target. How much they want to make. 
            new StreakGambler( "Camille", 200, 10, 10, 2, 0.5, 500 ),
        ]);

        this._maxRounds = maxRounds;
        this._currentRound = 0;
    }

    

    /**
     * Add profit to the casino for the day.
     * @param amount The amount of profit to add. If negative, it counts as a
     * loss.
     */
    public addProfit( amount: number ): void {
        this._profits += amount;
    }

    /** For each game: have each gambler who is still present play.
     * Starts by printing how much money each gambler has. 
     * If a gambler runs out of money or hits their target, they leave.
     * Then, plays the game with all players.
     */
    public simulateOneRound(): void {
        const startingProfit = this._profits;

        console.log( "-----------------------" );
        console.log( "beginning round", this._currentRound );
        for( let game of this._games ) {
            this.determineWhoIsStillPlaying();

            // add each player who is still playing to the game.
            // have them use the bet size determined by their personality.
            for( let player of this._gamblers ) {
                game.addPlayer( player, player.getBetSize() );
            }

            const gameStartingProfit = this._profits;
            game.playGame();
            console.log( 
                "casino made", 
                casino._profits - gameStartingProfit, "on this game.")
            console.log();
        }
        console.log( 
            "round complete. casino made: ", this._profits - startingProfit );
        console.log( "total profit:", this._profits );
        console.log( "-----------------------" );
    }

    /**
     * Run the simulation until either the maximum number of games is reached,
     * or no one is left in the casino.
     */
    public simulate(): void {
        while( this._currentRound < this._maxRounds && this._gamblers.size > 0 ) {
            this.simulateOneRound();
            console.log();
            this._currentRound++;
        }

        console.log( "simulation complete" );
    }

    /**
     * Update and list the people who are still playing.
     */
    private determineWhoIsStillPlaying() {
        const gamblersWhoLeft: Gambler[] = [];
        
        // update and list of who is still playing
        for( let gambler of this._gamblers.keys() ) {
            console.log( gambler.name, ": ", gambler.money );
            
            if( gambler.isFinished() ) {
                // add this person to the list of gamblers to remove.
                // don't remove it right away: removing an element from a 
                // collection that we are iterating over is usually a bad
                // idea.
                gamblersWhoLeft.push( gambler );
            }

            // now, print why the person left if they did so
            if( gambler.hitTarget() ) {
                console.log( 
                    gambler.name, 
                    "has hit their target! They leave the casino..."
                );
            }
            else if( gambler.bankrupt() ) {
                console.log( 
                    gambler.name,
                    "has gone bankrupt! They leave the casino..."
                );
            }
        }

        // remove the gamblers who left from the set
        for( let leaver of gamblersWhoLeft ) {
            this._gamblers.delete( leaver );
        }
    }
}

const MAX_N_ROUNDS = 5;

// main:
const casino = new Casino( MAX_N_ROUNDS );

casino.simulate();
