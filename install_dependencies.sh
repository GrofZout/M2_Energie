#!/bin/bash

# Mettre à jour la liste des paquets
sudo apt update

# Installer les paquets système nécessaires
sudo apt install -y openjdk-21-jdk-headless gcc libopenmpi-dev python3 python3-dev libomp-dev

# Vérifier si pip est installé, sinon l'installer
if ! command -v pip &> /dev/null
then
    echo "pip n'est pas installé. Installation de pip..."
    sudo apt install -y python3-pip
fi

# Installer les bibliothèques Python nécessaires
sudo pip install mpi4py matplotlib numpy argparse pandas

echo "Installation terminée ! Tous les paquets et bibliothèques nécessaires sont installés."

echo 1400000 | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_max_freq

echo "Fréquence max 1,4GHz"
