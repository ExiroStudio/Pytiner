pytineron() {
    source .pytiner_env/bin/activate
    cd code/
    echo "[Pytiner] Activated and moved to code/"
}

pytineroff() {
    deactivate
    cd ..
    echo "[Pytiner] Deactivated and returned to project root"
}
