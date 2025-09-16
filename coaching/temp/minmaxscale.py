import pandas as pd
from sklearn.preprocessing import MinMaxScaler

# Load the dataset (no headers in the raw file)
url = "..\\ml-ai\\framework\\ml-framework-1\\datasets\\diabetes.csv"
column_names = [
    "Pregnancies","Glucose","BloodPressure","SkinThickness",
    "Insulin","BMI","DiabetesPedigreeFunction","Age","Outcome"
]
df = pd.read_csv(url, names=column_names)

print(df.head())

X = df.drop("Outcome", axis=1)  # Features
y = df["Outcome"]               # Target variable


scaler = MinMaxScaler(feature_range=(0, 1))  # default range is 0–1
X_scaled = scaler.fit_transform(X)

# Convert back to a DataFrame (optional, for readability)
X_scaled_df = pd.DataFrame(X_scaled, columns=X.columns)

print("\nBefore scaling (first 5 rows):\n", X.head())
print("\nAfter Min-Max scaling (first 5 rows):\n", X_scaled_df.head())

# Save the scaled features along with the Outcome column
scaled_with_target = pd.concat([X_scaled_df, y], axis=1)

scaled_with_target.to_csv("..\\ml-ai\\framework\\ml-framework-1\\datasets\\diabetes_scaled.csv", index=False)

print("Scaled dataset saved to pima_scaled.csv")
 