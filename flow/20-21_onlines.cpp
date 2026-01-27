// 21 onlines

// A sec -> simple bipartite matching


// B sec ->
// You have M tasks and N workers. Each task requires an amount of strength to complete the
// task. Each of the workers has a strength. You’re given the strength of the workers and the
// strength each job required to be completed. One worker can only do one task. Find the
// maximum number of tasks that can be assigned to the workers.

// workers on left -> connect to source with cap = 1
// jobs on rights -> connect to sink with cap = 1
// add edge btwn worker[i] and job[j] iff. strength(worker) > strength(job)
// edge cap = 1


// C sec ->
// similar to 22 C sec online



// 20 onlines

// A sec -> lula


// B sec -> 
// A potato chips company is giving a sticker for free with each of their chips packets.
// But the sticker resides inside the packet, so there is no way to know which sticker 
// you are getting while buying a potato chips. The chips company is putting m different 
// stickers into their packets.

// As a kid, you are obsessed with these stickers and you want to collect as many 
// different stickers as possible. All of your n friends want the same as well.

// One person can have duplicates of a certain sticker. Everyone trades duplicates
// for stickers he doesn’t possess. Since all stickers have the same value, 
// the exchange ratio is always 1:1.

// As a clever kid, you have realized that in some cases it is good for you 
// to trade one of your duplicate stickers for a sticker you already possess. 
// This may seem counter-intuitive, as you will have more duplicate stickers 
// after such exchange. But for future trades, these duplicates may come handy.
// As your friends aren’t as clever as you, you can assume that your friends 
// will only exchange stickers with you and they will give away only duplicate 
// stickers in exchange with different stickers they don’t possess.


// Edges
// Source → sticker node
// Capacity = number of that sticker you have
// Why? You can “send” your stickers to trades
// Sticker → friend node
// Capacity = 1 if that friend doesn’t have that sticker
// Why? You can give this sticker to the friend (if they don’t have it)
// Friend → sticker node
// Capacity = number of that sticker friend has minus 1
// Why? Friend only trades duplicates → they must keep at least one of each type
// Sticker → dummy sink
// Capacity = 1
// Why? Only one copy of each sticker is needed for your collection


// C sec -> 
// In the annual cultural program, school ABC decides to organize dances in pairs. 
// Every pair of students (one boy, one girl) who wants to dance must register in advance.
// School regulations limit each boy-girl pair to at most m dances together, and 
// limit each student to at most n dances overall. We want to maximize the number of dances. 
// (Usually n is larger than m.)

// This is a binary assignment problem for the set X of girls and the set Y of boys.

// We construct a flow network G = (V, E) with vertices X ∪ Y ∪ {s, t} and the following edges:
// • an edge s → x with capacity n for each x ∈ X
// • an edge y → t with capacity n for each y ∈ Y
// • (if x and y registered to dance) an edge x → y with capacity m for each x ∈ X and y ∈ Y
// • If x and y didn’t register to dance, edge x → y has a capacity 0
