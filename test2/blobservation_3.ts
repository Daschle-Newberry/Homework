class MyBlob {
    private x: number;
    private y: number;
    private size: number;

    public constructor(x: number, y: number, size: number) {
        this.x = x;
        this.y = y;
        this.size = size;
    }

    public getMove(grid: (MyBlob | undefined)[][], h: number, w: number): [number, number] {
        let done: boolean = false;
        
        let target: MyBlob | undefined;
        
        let radius: number = 0;

        while(!done) {
            const halfDistance: number = Math.floor((3 + (2 * radius))/2);

            const leftRaw: number = this.x - halfDistance;
            const rightRaw: number = this.x + halfDistance;
            const topRaw: number = this.y - halfDistance;
            const bottomRaw: number = this.y + halfDistance;

            const l: number = Math.max(this.x - halfDistance, 0)
            const r: number = Math.min(this.x + halfDistance, w - 1);
            const t: number = Math.max(this.y - halfDistance, 0);
            const b: number = Math.min(this.y + halfDistance, h - 1);
            
            if(t == 0 && b == h - 1 && l == 0 && r == w - 1) done = true;
            if(topRaw >= 0 || bottomRaw < h) {
                for(let i = l; i <= r; i++) {
                    if(topRaw >= 0 && this.isValidTarget(grid[topRaw][i],target)) { target = grid[topRaw][i]; done = true; }
                    if(bottomRaw < h && this.isValidTarget(grid[bottomRaw][i],target)) { target = grid[bottomRaw][i]; done = true; }
                }
            }

            if(leftRaw >= 0 || rightRaw < w) {
                for(let i = t + 1; i <= b; i++) {
                    if(leftRaw >= 0 && this.isValidTarget(grid[i][leftRaw],target)) { target = grid[i][leftRaw]; done = true; }
                    if(rightRaw < w && this.isValidTarget(grid[i][rightRaw],target)) { target = grid[i][rightRaw]; done = true; }
                }
            }
            radius++;
        }

        if(target == undefined) return [0,0];
        

        const dx: number = target.getX() - this.x;
        const dy: number = target.getY() - this.y;
        const stepX = dx == 0 ? 0 : dx/Math.abs(dx);
        const stepY = dy == 0 ? 0 : dy/Math.abs(dy);

        return [stepX, stepY];
    }

    private isValidTarget(blob: MyBlob | undefined, current: MyBlob | undefined): boolean {
        return blob !== undefined && (blob.getSize() < this.size) && (current === undefined || (blob.getSize() > current.getSize()));
    }
    public getX(): number { return this.x; }
    public getY(): number { return this.y; }

    public getSize(): number { return this.size; }
    public addSize(delta: number) { this.size += delta; }

    public setX(x: number): void { this.x =x; }
    public setY(y: number): void { this.y =y; }


}


class Blobservation {
    private h: number;
    private w: number;
    private blobs: Set<MyBlob>;
    private grid: (MyBlob | undefined)[][];
    public constructor(h: number, w: number = h) {
        this.h = h;
        this.w = w;
        
        this.blobs = new Set();
        this.grid = [];
        for(let y = 0; y < this.h; y++) {
            this.grid.push(new Array());
            for(let x = 0; x < this.w; x++) {
                this.grid[y].push(undefined);
            }   
        }
    }

    public populate(blobs: {x: number, y: number, size: number}[]): void {
        for(const blob of blobs) {
            const current: MyBlob | undefined = this.grid[blob.y][blob.x];
            if(current == undefined) {
                const blobObj: MyBlob = new MyBlob(blob.x, blob.y,blob.size);
                this.grid[blob.y][blob.x] = blobObj;
                this.blobs.add(blobObj);
            } else {
                current.addSize(blob.size);
            }
        }
    }

    public move(iters: number = 1): void {
        while(iters > 0) {

            const moves: Map<MyBlob, {x: number, y: number}> = new Map();
            for(const blob of this.blobs) {
                const [stepX,stepY] = blob.getMove(this.grid, this.w, this.h);
                const x = stepX + blob.getX();
                const y = stepY + blob.getY();
                
                moves.set(blob, {x:x, y:y});
            }

            this.resetGrid();

            for(const [blob,move] of moves) {
                blob.setX(move.x);
                blob.setY(move.y);
                this.mergeCells(blob,blob.getX(),blob.getY());

            }

            iters--;
        }
    }

    public print_state(): number[][] {
        const sorted: MyBlob[] = [...this.blobs].sort((a: MyBlob,b: MyBlob) => {
            if(a.getX() !== b.getX()) {
                return a.getX() - b.getX();
            }
            return a.getY() - b.getY();
        });
        
        return sorted.map((blob: MyBlob) => [blob.getX(),blob.getY(),blob.getSize()]);;
    }

    public printGrid(): void {
        for(let y = 0; y < this.h; y++) {
            for(let x = 0; x < this.w; x++) {
                const cell: MyBlob | undefined = this.grid[y][x];
                const pos: String = cell == undefined ? "-" : cell.getSize().toString();

                process.stdout.write(`${pos} `);
            }
            console.log();
        }
    }

    private resetGrid(): void {
        this.grid = [];
        for(let y = 0; y < this.h; y++) {
            this.grid.push(new Array());
            for(let x = 0; x < this.w; x++) {
                this.grid[y].push(undefined);
            }   
        }
    }
    private mergeCells(blob: MyBlob, x: number, y: number) {
        const cell: MyBlob | undefined = this.grid[y][x]
        if(cell === undefined) this.grid[y][x] = blob;
        else {
            //Doesn't matter which blob gets deleted
            console.log(`BLOBS ${blob.getSize()} and ${cell.getSize()} merge`)
            this.blobs.delete(blob);
            cell.addSize(blob.getSize());
        }
    }
}