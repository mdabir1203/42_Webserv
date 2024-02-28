use reqwest::Client;
use std::error::Error;
use serde::Deserialize;
use serde_json::json;

#[derive(Deserialize, Debug)]
struct ApiResponse {
    data: Vec<ImageData>,
}

#[derive(Deserialize, Debug)]
struct ImageData {
    url: String,
}


// Steps to understand this code:
// 1. Prepare: Import necessary libraries.
// 2. Setup: Replace placeholders with actual API details.
// 3. Compose: Organize the information you want to send in a structured way.
// 4. Send: Use the tools to send your request to the API.
// 5. Receive: Await the response and deal with any issues.
// Allowing asynchronous execution of the main fnc
#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    let client = Client::new();
    // Create the payload as a serde_json::Value
    let payload = json!({
        "model": "dall-e-3",
        "prompt": "a linkedin carousel banner with inserted {text}",
        "size": "1024x1024",
        "quality": "standard",
        "n":  1,
    });

    let response = client.post("https://api.openai.com/v1/images/generations")
        .header("Authorization", "API_Key")
        .json(&payload) // Use the payload here
        .send()
        .await?;

    let api_response: ApiResponse = response.json().await?;
    let image_url = &api_response.data[0].url;
    println!("Image URL: {}", image_url);

    Ok(())
}


