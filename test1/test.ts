//Problem Domain: Modeling a parking lot/Managing a parking lot?


interface Vehicle {
    park(lot: ParkingLot): void;
    leave(lot: ParkingLot): void;
}

type ParkingSpot = {row: number, col: number}

class Ferrari implements Vehicle {
    //Name of the driver
    private name: string;

    //Current parking spot, if any
    private spot: ParkingSpot | undefined;

    public constructor(name: string) {
        this.name = name;
    }

    public park(lot: ParkingLot): void {
        //Ferrari looks for spots further away from the entrance
        for(let row = lot.getRows() - 1; row >= 0; row--) {
            for(let col = lot.getCols() - 1; col >= 0; col--) {
                if(lot.requestSpot(this,row,col)) {
                    this.spot = {row:row, col:col};
                    return;
                }
            }
        }
    }

    //Tries to remove itself from its parking spot, if the car is gone is it assumed to be stolen.
    public leave(lot: ParkingLot): void {
        if(this.spot === undefined) return;

        if(!lot.leaveSpot(this,this.spot.row, this.spot.col)) {
            console.log(`${this.name}: Someone stole my car... I guess I'll just buy a new one!`)
        } else {
            console.log(`${this.name}: Ew, did a prius park next to me?!`)
        }
    }
}

class Truck implements Vehicle {
    private name: string;
    private spots: ParkingSpot[];

    public constructor(name: string) {
        this.spots = [];
        this.name = name;
    }

    public park(lot: ParkingLot): void {
        //Truck will search for a spot and take up to 3 parking spaces
        let count: number = 0;

        for(let row = 0; row < lot.getRows() && count == 0; row++) {
            this.spots = [];
            for(let col = 0; col < lot.getCols(); col++) {
                if(lot.requestSpot(this, row, col)) {
                    count++;
                    this.spots.push({row:row, col:col});
                    if(count > 2) return;
                } else if(count > 0) {
                    return;
                }
            }
        }
    }

    //Tries to remove itself from its parking spot, if the car is gone is it assumed to be stolen. 
    // If the car is not in all of the original spaces, it is assumed to have been moved.
    public leave(lot: ParkingLot): void {
        let found: boolean = false;

        let moved: boolean = false;
        for(const spot of this.spots) {
            let inSpot: boolean = lot.leaveSpot(this,spot.row, spot.col);
            found = found || inSpot;
            moved = !(moved !== inSpot);
        }
        
        if(!found) console.log(`${this.name}: Someone stole my damn truck!`);
        else if(moved) console.log(`${this.name}: Someone moved my damn truck!`);
        else{
            console.log(`${this.name}: I love my damn truck!`);
        }
    }
}


class ParkingLot {
    //2D array of spots
    private parkingSpots: Array<Array<Vehicle | undefined>>;
    private rows: number;
    private cols: number;

    public constructor(rows: number, cols: number) {
        this.parkingSpots = new Array();
        this.rows = rows;
        this.cols = cols;

        //init empty spots
        this.parkingSpots = new Array(rows);
        for(let i = 0; i < rows; i++) {
            this.parkingSpots[i] = new Array(cols);
        }
    }

    //Puts the vehicle into the requested spot and returns true. If spot is full or invalid, returns false.
    public requestSpot(vehicle: Vehicle, row: number, col: number): boolean {
        if(!this.isValidSpot(row,col)) return false;

        if(this.parkingSpots[row][col] === undefined) {
            this.parkingSpots[row][col] = vehicle;
            return true 
        }else {
            return false;
        }
    }

    //Removes vehicle from requested spot and returns true. If spot is invalid or the vehicle is not in that spot, returns false.
    public leaveSpot(vehicle: Vehicle, row: number, col: number): boolean {
        if(!this.isValidSpot(row,col)) return false;

        if(this.parkingSpots[row][col] !== vehicle){ 
            return false;
        } else{
            this.parkingSpots[row][col] = undefined
            return true;
        }
    }

    //Bounds check
    private isValidSpot(row: number, col: number) {
        return (0 < row && row < this.rows) || (0 < this.cols && col < this.cols);
    }

    public getRows(): number {return this.rows;}
    public getCols(): number {return this.cols;}
}


const parkingLot : ParkingLot = new ParkingLot(5,5);

let vehicles: Array<Vehicle> = new Array(10);

const ferarriaNames: string[] = ["Casey", "Hannah", "Chris", "Margit, The Fell Omen", "John"]
const truckNames: string[] = ["Jim", "Mary","Bryson","Shirley","Robert"]

for(let i = 0; i < 5; i++) {
    vehicles[i] = new Truck(truckNames[i]);
    vehicles[i + 5] = new Ferrari(ferarriaNames[i]);
}

for(const vehicle of vehicles) {
    vehicle.park(parkingLot);
}

for(const vehicle of vehicles) {
    vehicle.leave(parkingLot);
}


