import requests
from bs4 import BeautifulSoup
import csv
import re

# Step 1: Fetch the web page
url = 'https://www.timeanddate.com/sun/canada/victoria' # This URL will update to the current month.

# To select a different month, change the URL to the desired month.
# For example, to get the data for October 2021, change the URL to ?month=3, where in this case, 3 is May: 
# url = 'https://www.timeanddate.com/sun/canada/victoria?month=3'

response = requests.get(url)
soup = BeautifulSoup(response.text, 'html.parser')

# Step 2: Find all table rows containing the 'Click to expand for more details' in their 'title' attribute
rows = soup.find_all('tr', title="Click to expand for more details")

# Step 3: Create a list to hold extracted data
table_data = []

# Function to clean symbols from a string (e.g., remove arrows, degrees, parentheses content)
def clean_text(text):
    # Remove any arrows (↑), degree symbols (°), and content inside parentheses (and the parentheses)
    cleaned_text = re.sub(r'[↑°]', '', text)  # Remove ↑ and °
    cleaned_text = re.sub(r'\(.*?\)', '', cleaned_text)  # Remove text within parentheses
    return cleaned_text.strip()  # Remove extra spaces

print("SunData monthData[] = {")
# Step 4: Extract the required columns from each row
for row in rows:
    columns = row.find_all(['th', 'td'])  # Get all <th> and <td> elements
    day = columns[0].text.strip()  # Day of the month (1-31)

    sunrise = clean_text(columns[1].text.strip())
    sunrise_hour = int(sunrise[:-2].split(':')[0])
    sunrise_minute = int(sunrise[:-2].split(':')[1])

    if sunrise[-2:] == "pm" and sunrise_hour != 12:
        sunrise_hour += 12

    sunset = clean_text(columns[2].text.strip())
    sunset_hour = int(sunset[:-2].split(':')[0])
    sunset_minute = int(sunset[:-2].split(':')[1])

    # Convert to 24 hour format. Makes math way easier in the arduino.
    if sunset[-2:] == "pm" and sunset_hour != 12:
        sunset_hour += 12

    solar = clean_text(columns[11].text.strip())
    solar_hour = int(solar[:-2].split(':')[0])
    solar_minute = int(solar[:-2].split(':')[1])

    if solar[-2:] == "pm" and solar_hour != 12:
        solar_hour += 12
    distance = columns[12].text.strip()

    month = 'Oct'

    print("\t{", end="")
    print(f"{day}, {sunrise_hour}, {sunrise_minute}, {sunset_hour}, {sunset_minute}, {solar_hour}, {solar_minute}", end="")
    print("},")

    # Append the cleaned data as a row
    table_data.append([month, day, sunrise, sunrise_hour, sunrise_minute, sunset, sunset_hour, sunset_minute, solar, solar_hour, solar_minute])
print("};")


# Step 5: Write the cleaned data to a CSV file
with open('sun_data.csv', mode='w', newline='', encoding='utf-8') as file:
    writer = csv.writer(file)
    writer.writerow(['Month', 'Day', 'Sunrise', 'Sunrise Hour', 'Sunrise Minute', 'Sunset Hour', 'Sunset Minute', 'Solar Noon Hour', 'Solar Noon Minute'])

    
    # Write the table rows
    writer.writerows(table_data)

print("CSV file 'sun_data.csv' has been created with cleaned data.")

