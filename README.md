🔐 LSB Image Steganography (C Project)

📌 Overview
- LSB Image Steganography is a technique used to hide secret information inside digital images.
- It ensures that communication remains hidden by concealing data within another file.
- Unlike cryptography, it hides the existence of the message itself.
- Uses BMP images as the carrier for embedding data.

🎯 Objective
- Design a command-line application for data hiding and retrieval.
- Implement secure and invisible message embedding.
- Provide decoding functionality to extract hidden data.

🧠 Concept
- Based on Least Significant Bit (LSB) manipulation.
- Modifies the smallest bit of image pixels to store data.
- Changes are not visible to the human eye.
- Exploits limitations of human visual perception.

⚙️ Features

🔒 Encoding
- Accepts `.bmp` image as input.
- Accepts `.txt` file containing secret message.
- Checks if image capacity is sufficient.
- Embeds:
  - Magic string
  - File extension
  - File size
  - File data
- Generates a stego image.

🔓 Decoding
- Extracts hidden data from stego image.
- Verifies presence using magic string.
- Reconstructs original message file.

🧾 Requirements
- Input image format: `.bmp`
- Secret file format: `.txt`
- Language: C
- Platform: Command-line (Linux/Windows)

🧪 Working Flow

### Encoding Process
- Read input BMP image.
- Read secret text file.
- Validate image capacity.
- Encode:
  - Magic string
  - File metadata
  - File content
- Generate output stego image.

### Decoding Process
- Read stego image.
- Extract magic string.
- Validate hidden data presence.
- Decode file metadata.
- Recover original message.

⚡ Advantages
- High invisibility of hidden data.
- Simple and efficient implementation.
- Fast execution with low resource usage.

⚠️ Limitations
- Limited storage capacity based on image size.
- Not suitable for compressed formats like JPEG.
- Less secure without encryption.

🚀 Applications
- Secure communication
- Digital watermarking
- Data hiding in multimedia
- Confidential file transfer
