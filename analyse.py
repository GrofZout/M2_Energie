import subprocess
import pandas as pd
import time



# Charger le fichier Excel .ods

ods_file = 'Système_Embarqué_Énergie.ods'

sheet_data = pd.read_excel(ods_file, sheet_name=None, engine='odf')


# Fonction pour calculer l'énergie totale à partir de data.csv, en ignorant la première ligne
def calculer_energie_totale(csv_file):
    # Lire le fichier CSV
    data = pd.read_csv(csv_file, delim_whitespace=True, comment='#')  # `delim_whitespace` pour gérer les espaces
    data = data.iloc[1:]  # Ignorer la première ligne
    consommation_totale = data.iloc[:, 1:].sum().sum() / 1e5  # µJ -> J
    return consommation_totale


for sheet_name, df in sheet_data.items():
    print(f"\n=== Traitement de la feuille : {sheet_name} ===")
    for index, row in df.iterrows():
        version = row.get("Version", "Version inconnue")
        taille = row.get("Taille", "Taille inconnue")
        iterations = row.get("Itérations", "Itérations inconnues")
        
        print(f"\n>> Début de l'exécution pour la version : {version}")
        print(f"🔍 Paramètres : Taille = {taille}, Itérations = {iterations}")
        
        # Récupérer la commande de compilation si nécessaire
        compile_command = row.get("Ligne de compilation si besoin")
        if pd.notna(compile_command):
            print(f"🔧 Compilation avec la commande : {compile_command}")
            compile_result = subprocess.run(compile_command, shell=True, capture_output=True, text=True)
            if compile_result.returncode == 0:
                print("✅ Compilation réussie !")
            else:
                print("❌ Erreur lors de la compilation :")
                print(compile_result.stderr)
                continue

        # Récupérer la commande d'exécution
        execute_command = row.get("Ligne de commande")
        if pd.notna(execute_command):
            print(f"🚀 Exécution avec la commande : {execute_command}")
            start_time = time.time()  # Début du chronomètre
            mojitos = subprocess.Popen(["./mojitos/mojitos", "-r", "-f", "2", "-o", "data.csv"])
            execute_result = subprocess.run(execute_command, shell=True, capture_output=True, text=True)
            mojitos.terminate()
            end_time = time.time()  # Fin du chronomètre
            execution_time = end_time - start_time
            
            if execute_result.returncode == 0:
                print("🎉 Exécution réussie ! Voici la sortie :")
                print(execute_result.stdout)
                print(f"⏱️ Temps d'exécution : {execution_time:.2f} secondes")
                
                # Calculer l'énergie totale consommée
                energie_totale = calculer_energie_totale('data.csv')
                print(f"⚡ Consommation totale : {energie_totale:.2f} J")

                # Calculer la puissance maximale
                if execution_time > 0:
                    puissance_max = energie_totale / execution_time
                else:
                    puissance_max = 0.0
                print(f"🔋 Puissance maximale : {puissance_max:.2f} W")
                
                # Mettre à jour les colonnes "Énergie (J)" et "Temps (s)" dans le tableau
                # Mettre à jour les colonnes dans le tableau
                sheet_data[sheet_name].at[index, "Énergie (J)"] = energie_totale
                sheet_data[sheet_name].at[index, "Temps (s)"] = execution_time
                sheet_data[sheet_name].at[index, "Puissance maximum (W)"] = puissance_max
            else:
                print("❌ Erreur lors de l'exécution :")
                print(execute_result.stderr)
                print(f"⏱️ Temps d'exécution avant échec : {execution_time:.2f} secondes")

        print(f">> Fin de l'exécution pour la version : {version}\n")

# Sauvegarder le fichier ODS mis à jour
output_ods_file = 'Système_Embarqué_Énergie_MisAJour.ods'
with pd.ExcelWriter(output_ods_file, engine='odf') as writer:
    for sheet_name, df in sheet_data.items():
        df.to_excel(writer, sheet_name=sheet_name, index=False)
