export function randInRange(min: number, max: number, round: boolean = false): number {
	let rand: number = Math.random() * (max - min) + min
	if(round) rand = Math.round(rand);
	return rand;
}

export function weightedRandomIndex(indexWeights: number[]): number {
	const ranges: number[] = [];

	let sum: number = 0;
	for(let i = 0; i < indexWeights.length; i++) {
		ranges[i] = indexWeights[i] + sum;
		sum += indexWeights[i];

		if(sum > 1) throw new Error(`Supplied weights add up to more than 1 ${indexWeights}`);
	}
	if(sum < 1) throw new Error(`Supplied weights do not add up to 1 ${indexWeights}`);


	const selection: number = Math.random();

	let last: number = 0;

	for(let i = 0; i < ranges.length; i++) {
		if(last < selection && selection <= ranges[i]) return i;
	}

	return -1;
}