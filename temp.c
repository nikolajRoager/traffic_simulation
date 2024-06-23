for (X = parent(Z); X != null; X = parent(Z)) { // Loop (possibly up to the root)
    // BF(X) has to be updated:
    if (Z == right_child(X)) { // The right subtree increases
        if (BF(X) > 0) { // X is right-heavy
            // ==> the temporary BF(X) == +2
            // ==> rebalancing is required.
            G = parent(X); // Save parent of X around rotations
            if (BF(Z) < 0)                  // Right Left Case  (see figure 3)
                N = rotate_RightLeft(X, Z); // Double rotation: Right(Z) then Left(X)
            else                            // Right Right Case (see figure 2)
                N = rotate_Left(X, Z);      // Single rotation Left(X)
            // After rotation adapt parent link
        } else {
            if (BF(X) < 0) {
                BF(X) = 0; // Z’s height increase is absorbed at X.
                break; // Leave the loop
            }
            BF(X) = +1;
            Z = X; // Height(Z) increases by 1
            continue;
        }
    } else { // Z == left_child(X): the left subtree increases
        if (BF(X) < 0) { // X is left-heavy
            // ==> the temporary BF(X) == -2
            // ==> rebalancing is required.
            G = parent(X); // Save parent of X around rotations
            if (BF(Z) > 0)                  // Left Right Case
                N = rotate_LeftRight(X, Z); // Double rotation: Left(Z) then Right(X)
            else                            // Left Left Case
                N = rotate_Right(X, Z);     // Single rotation Right(X)
            // After rotation adapt parent link
        } else {
            if (BF(X) > 0) {
                BF(X) = 0; // Z’s height increase is absorbed at X.
                break; // Leave the loop
            }
            BF(X) = -1;
            Z = X; // Height(Z) increases by 1
            continue;
        }
    }
    // After a rotation adapt parent link:
    // N is the new root of the rotated subtree
    // Height does not change: Height(N) == old Height(X)
    parent(N) = G;
    if (G != null) {
        if (X == left_child(G))
            left_child(G) = N;
        else
            right_child(G) = N;
    } else
        tree->root = N; // N is the new root of the total tree
    break;
    // There is no fall thru, only break; or continue;
}
// Unless loop is left via break, the height of the total tree increases by 1.
