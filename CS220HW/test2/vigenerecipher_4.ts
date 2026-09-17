
//https://www.codewars.com/kata/52d1bd3694d26f8d6e0000d3 4 kyu
// Used compiled javascript to submit

function mod(x: number, n: number) {
    return ((x % n) + n) % n;
}

class VigenèreCipher {
    private key: string;
    private alphabet: string;


    public constructor(key: string, alphabet: string) {
        this.key = key;
        this.alphabet = alphabet;

        console.log(key);
        console.log(alphabet);
    }


    public encode(message: string): string {
        const shifts: Map<string, number> = new Map();
        this.alphabet.split("").forEach((c: string, i: number) => {shifts.set(c, i)});

        let res = "";

        message.split("").forEach((c: string, i: number) => {
            const charPos: number | undefined = shifts.get(c);
            const shift: number | undefined = shifts.get(this.key.charAt(i % this.key.length));

            if(charPos == undefined || shift == undefined) res += c;
            else res += this.alphabet.charAt((charPos + shift) % this.alphabet.length);

        })
        return res;
    }

    public decode(cipher: string): string {
        const shifts: Map<string, number> = new Map();
        this.alphabet.split("").forEach((c: string, i: number) => {shifts.set(c, i)});

        let res = "";

        cipher.split("").forEach((c: string, i: number) => {
            const charPos: number | undefined = shifts.get(c);
            const shift: number | undefined = shifts.get(this.key.charAt(i % this.key.length));

            if(charPos == undefined || shift == undefined) res += c;
            else res += this.alphabet.charAt(mod((charPos - shift), this.alphabet.length));

        })
        return res;
        }
    }


const abc = "abcdefghijklmnopqrstuvwxyz";
const key = "pizza"
const c = new VigenèreCipher(key, abc);

console.log(c.encode("it's a shift cipher!"));
console.log(c.decode(c.encode("it's a shift cipher!")));